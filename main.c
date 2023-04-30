#include <stdio.h>
#include <stdbool.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void print_version();
static void print_help();

static int compile_image(const char *input, const char *output);
static int parse_argument(char *arg, int index, int argc, char **argv);

static void write_pixel_value(FILE *outfile, unsigned char *image_data, int x, int y, int width, int height, int channels);

static char *output_file;
static char *input_file;
static enum {
	FORMAT_XY,
	FORMAT_YX
} pixel_output_format = FORMAT_XY;

int main(int argc, char **argv)
{
	char *current_arg;
	int i;

	if (argc <= 1)
	{
		fprintf(stderr, "Usage: %s -i <input file> -o <output file>\n", basename(argv[0]));
		return 1;
	}

	for (i = 1; i < argc; i++)
	{
		current_arg = argv[i];

		if (current_arg[0] == '-')
		{
			i += parse_argument(current_arg + 1, i, argc, argv);
		}
	}

	if (input_file == NULL)
	{
		fprintf(stderr, "Please specify an input file\n");
		return 1;
	}

	if (output_file == NULL)
	{
		fprintf(stderr, "Please specify an output file\n");
		return 1;
	}

	return compile_image(input_file, output_file);
}

static void print_version()
{
	puts("PNG 2 C Compiler");
	puts("version: 0.1-dev");
}

static void print_help()
{
	print_version();
	puts("");

	puts("Usage:");
	puts("\t-i\t\tThe input file used to convert");
	puts("\t-o\t\tThe output file to write after converting");
	puts("\t-f\t\tSet the pixel indexing format (use -f -l to list all)");
	puts("\t-v\t\tShow the program version");
	puts("\t-h\t\tShow this help menu");
	puts("");
}

static int compile_image(const char *input, const char *output)
{
	int width, height, channels, image_size, x, y;
	unsigned char *image_data;
	FILE *outfile;

	image_data = stbi_load(input, &width, &height, &channels, 0);

	if (image_data == NULL)
	{
		fprintf(stderr, "Could not load image from input file %s: %s\n", input, stbi_failure_reason());

		return 1;
	}

	errno = 0;
	outfile = fopen(output, "w");

	if (outfile == NULL)
	{
		fprintf(stderr, "Could not open output file %s: %s\n", output, strerror(errno));

		stbi_image_free(image_data);

		return 1;
	}

	char *header_name = strtok(basename((char *) output), ".");

	/** add ifdef guard */
	fprintf(outfile, "#ifndef __%s_h__\n"  , header_name);
	fprintf(outfile, "#define __%s_h__\n\n", header_name);

	/** create variable definition */
	fprintf(
		outfile,
		"struct {\n"
		"\tint width, height, format;\n"
		"\tunsigned int pixels[%d][%d];\n"
		"} %s_data = {\n"
		"\t.width = %d,\n"
		"\t.height = %d,\n"
		"\t.format = %d,\n"
		"\t.pixels = {",
		width,
		height,
		header_name,
		width,
		height,
		(int) pixel_output_format);

	
	switch (pixel_output_format)
	{
	case FORMAT_XY:
		for (x = 0; x < width; x++)
		{
			fprintf(outfile, "{");
			for (y = 0; y < height; y++)
			{
				write_pixel_value(outfile, image_data, x, y, width, height, channels);

				if (y + 1 < height)
				{
					fputc(',', outfile);
				}
			}
			fprintf(outfile, "}");

			if (x + 1 < width)
			{
				fputc(',', outfile);
			}
		}
		break;
	case FORMAT_YX:
		for (y = 0; y < height; y++)
		{
			fprintf(outfile, "{");

			for (x = 0; x < width; x++)
			{
				write_pixel_value(outfile, image_data, x, y, width, height, channels);

				if (y + 1 < height)
				{
					fputc(',', outfile);
				}
			}

			fprintf(outfile, "}");

			if (x + 1 < width)
			{
				fputc(',', outfile);
			}
		}
		break;
	}

	fprintf(outfile, "}\n};\n");

	/** end of ifdef guard */
	fprintf(outfile, "\n");
	fprintf(outfile, "#endif /** __%s_h__ */\n", header_name);

	fclose(outfile);
	stbi_image_free(image_data);

	return 0;
}

static void write_pixel_value(FILE *outfile, unsigned char *image_data, int x, int y, int width, int height, int channels)
{
	unsigned char r, g, b, a;
	int i, color;

	i = ((y * width) + x) * channels;

	r = (image_data[i]);
	g = (image_data[i + 1]);
	b = (image_data[i + 2]);
	a = channels > 3 ? image_data[i + 3] : 0xFF;

	color = ((r & 0xFF) << 24) |
		((g & 0xFF) << 16) |
		((b & 0xFF) <<  8) |
		(a & 0xFF);

	fprintf(outfile, "0x%x", color);
	
}

static int parse_argument(char *arg, int index, int argc, char **argv)
{
	switch (*arg)
	{
	case 'h':
		print_help();
		exit(0);

	case 'v':
		print_version();
		exit(0);

	case 'i':
		if (index + 1 >= argc)
		{
			fprintf(stderr, "Please supply an input to -i.\n");
			exit(1);
		}

		input_file = argv[index + 1];
		return 1;

	case 'o':
		if (index + 1 >= argc)
		{
			fprintf(stderr, "Please supply an input to -o\n");
			exit(1);
		}

		output_file = argv[index + 1];
		return 1;
	case 'f':
		if (index + 1 >= argc)
		{
			fprintf(stderr, "Please supply an input to -f\n");
			exit(1);
		}

		char *format = argv[index + 1];

		if (strcmp(format, "xy") == 0)
		{
			pixel_output_format = FORMAT_XY;
		}
		else if (strcmp(format, "yx") == 0)
		{
			pixel_output_format = FORMAT_YX;
		}
		else if (strcmp(format, "-l") == 0)
		{
			puts("Indexing order:");
			puts("\tyx\tpixels[y][x]");
			puts("\txy\tpixels[x][y]");
			puts("");
			exit(0);
		}
		else {
			fprintf(stderr, "Invalid format supplied!");
		}

		return 1;
	default:
		fprintf(stderr, "Unknown option -%c\n", *arg);
		exit(1);
	}

	return 0;
}
