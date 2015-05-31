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
	unsigned char b, n;

	if (argc != 4){
		printf("usage: pcm2c name foo.pcm foo.c\n");
		return 1;
	}

	toUppercase(uc_name, argv[1]);
	ifil = fopen(argv[2], "rb");
	ofil = fopen(argv[3], "w");

	fprintf(ofil,
		"#ifndef __%s_SND\n"
		"#define __%s_SND\n\n"
		"#include \"../audio.h\"\n\n"
		"// Use const to put the data in flash instead of SRAM.\n"
		"const uint8_t %s_data[] = \n{\n\t",
		uc_name, uc_name, argv[1]);

	while (1) {
		for (n=0; n<16; n++) {
			fread(&b, 1, sizeof(b), ifil);
			fprintf(ofil, "0x%02x", b);

			if (feof(ifil))
				break;
			else
				fprintf(ofil, ",");
		}

		if (feof(ifil))
			break;
		else
			fprintf(ofil, "\n\t");
	}

	fprintf(ofil,
		"\n};\n\n"
		"Sound %s_snd =\n{\n"
		"\t.pos = 0,\n"
		"\t.data = (uint8_t *)&%s_data,\n"
		"\t.len = sizeof(%s_data),\n"
		"};\n\n"
		"#endif // __%s_SND\n",
		argv[1], argv[1], argv[1], uc_name);

	fclose(ifil);
	fclose(ofil);
	return 0;
} 
