#include <stdio.h>
#include <string.h>

static void toUppercase(char *out, char *in)
{
	do {
		if (*in >= 'a' && *in <= 'z')
			*out = *in + 'A' - 'a';
		else
			*out = *in;

		in++;
		out++;
	} while (*in != '\0');
}

int main(int argc, char *argv[])
{
	FILE *ifil;
	FILE *ofil;
	char uc_name[32];
	unsigned char b[3], n;

	if (argc != 4){
		printf(
			"convert 24-bit raw image to 5-6-5 C header\n\n"
			"guide using GIMP:\n"
			"- use RGB palette\n"
			"- remove alpha layer\n"
			"- flip vertically and rotate 90\n"
			"- export in R-G-B raw format\n\n"
			"usage: text2c name in.raw out.c\n");
		return 1;
	}

	toUppercase(uc_name, argv[1]);
	ifil = fopen(argv[2], "rb");
	ofil = fopen(argv[3], "w");

	fprintf(ofil,
		"#ifndef __%s_TEX\n"
		"#define __%s_TEX\n\n"
		"// Use const to put the data in flash instead of SRAM.\n"
		"const uint8_t %s_tex_data[] = \n{\n\t",
		uc_name, uc_name, argv[1]);

	while (1) {
		for (n=0; n<4; n++) {
			unsigned short data;

			fread(&b, 3, sizeof(char), ifil);
			data = ((b[0] & 0xF8) << 8) | ((b[1] & 0xFC) << 3) | ((b[2] & 0xF8) >> 3);
			fprintf(ofil, "0x%02x, 0x%02x", data & 0xFF, (data >> 8) & 0xFF);

			if (feof(ifil))
				break;
			else
				fprintf(ofil, ", ");
		}

		if (feof(ifil))
			break;
		else
			fprintf(ofil, "\n\t");
	}

	fprintf(ofil,
		"\n};\n\n"
		"#endif // __%s_TEX\n",
		uc_name);

	fclose(ifil);
	fclose(ofil);
	return 0;
} 
