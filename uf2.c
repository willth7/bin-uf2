//   Copyright 2022 Will Thomas
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0;
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint8_t mag0[] = {85, 70, 50, 10};
uint8_t mag1[] = {87, 81, 93, 158};
uint8_t magz[] = {48, 111, 177, 10};

uint8_t z[476];

typedef struct uf2_s{
	uint32_t flags;
	uint32_t addr;
	uint32_t size;
	uint32_t ui;
	uint32_t un;
	uint32_t id;
} uf2_t;

int8_t main(int32_t argc, int8_t** argv) {
	FILE* f = fopen(argv[1], "r");
	if (f == 0) {
		printf("error: file %s doesn't exist\n", argv[1]);
		return -1;
	}
	fseek(f, 0, SEEK_END);
	uint64_t bn = ftell(f);
	uint8_t* bits = malloc(bn);
	fseek(f, 0, SEEK_SET);
	fread(bits, bn, 1, f);
	fclose(f);
	uint64_t bi = bn;
	
	uint64_t un = (bn / 256) + (!!(bn % 256));
	uf2_t* u = malloc(24 * un);
	
	void* buf = malloc(512 * un);
	
	for (uint8_t i = 0; i < un; i++) {
		u[i].flags = 8192;
		u[i].addr = 536870912 + (256 * i);
		u[i].size = 256;
		u[i].ui = i;
		u[i].un = un;
		u[i].id = 3834380118;
		
		memcpy(buf + (512 * i), mag0, 4);
		memcpy(buf + (512 * i) + 4, mag1, 4);
		memcpy(buf + (512 * i) + 8, u + i, 24);
		if (bi >= 256) {
			memcpy(buf + (512 * i) + 32, bits + (256 * i), 256);
			memcpy(buf + (512 * i) + 288, z, 220);
		}
		else {
			memcpy(buf + (512 * i) + 32, bits + (256 * i), bi);
			memcpy(buf + (512 * i) + 32 + bi, z, 476 - bi);
		}
		memcpy(buf + (512 * i) + 508, magz, 4);
		
		bi -= 256;
	}
	
	f = fopen(argv[2], "w");
	fwrite(buf, 512 * un, 1, f);
	fclose(f);
	
	free(u);
	free(bits);
	free(buf);
	
	return 0;
}
