// Ciupitu Dennis-Mircea 333CA
#include<mpi.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


float smoothK[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
float smoothFactor = 1.0f / 9.0f;

float blurK[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
float blurFactor = 1.0f / 16.0f;

float sharpenK[3][3] = {{0, -2, 0}, {-2, 11, -2}, {0, -2, 0}};
float sharpenFactor = 1.0f / 3.0f;

float meanK[3][3] = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}};
float meanFactor = 1.0f;

float embossK[3][3] = {{0, 1, 0}, {0, 0, 0}, {0, -1, 0}};
float embossFactor = 1.0f;

void applyFactor(float factor, float matrixK[3][3]) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			matrixK[i][j] *= factor;
		}
	}
}

void rotatedMatrix(float matrix[3][3]) {
	float aux;

	aux = matrix[0][0];
	matrix[0][0] = matrix[2][2];
	matrix[2][2] = aux;

	aux = matrix[0][1];
	matrix[0][1] = matrix[2][1];
	matrix[2][1] = aux;

	aux = matrix[0][2];
	matrix[0][2] = matrix[2][0];
	matrix[2][0] = aux;

	aux = matrix[1][0];
	matrix[1][0] = matrix[1][2];
	matrix[1][2] = aux;
}

void charToFloat(int width, int height, char** matrix, float** matrixToCast) {
	for (int i = 0; i <= height + 1; i++) {		
		for (int j = 0; j <= width + 1; j++) {
			matrixToCast[i][j] = (float)(matrix[i][j]);
		}
	}
}

void cloneMatrix(int width, int height, unsigned char** matrix, unsigned char** matrixClone){
	for (int i = 0; i <= height + 1; i++) {		
		for (int j = 0; j <= width + 1; j++) {
			matrixClone[i][j] = matrix[i][j];
		}
	}
}

void applyFilter(int width, int height, unsigned char **matrix, float matrixK[3][3], int maxvalue) {

	unsigned char ** filteredMatrix = (unsigned char **)malloc((height + 2) * sizeof(unsigned char *));
	for (int i = 0; i <= height + 1; i++) {	
		filteredMatrix[i] = (unsigned char *)malloc((width + 2) * sizeof(unsigned char));
	}

	cloneMatrix(width, height, matrix, filteredMatrix);

	for (int i = 1; i <= height; i++) {		
		for (int j = 1; j <= width; j++) {

			float floatResult = matrixK[0][0] * filteredMatrix[i - 1][j - 1] + matrixK[0][1] * filteredMatrix[i - 1][j] + matrixK[0][2] * filteredMatrix[i - 1][j + 1] +
							matrixK[1][0] * filteredMatrix[i][j - 1] + matrixK[1][1] * filteredMatrix[i][j] + matrixK[1][2] * filteredMatrix[i][j + 1] +
							matrixK[2][0] * filteredMatrix[i + 1][j - 1] + matrixK[2][1] * filteredMatrix[i + 1][j] + matrixK[2][2] * filteredMatrix[i + 1][j + 1];

			
			if (floatResult > (float)maxvalue) {
				floatResult = (float)maxvalue;
			}
			if (floatResult < 0.0f) {
				floatResult = 0.0f;
			}
			matrix[i][j] = (unsigned char)floatResult;
		}
		
	}

	for (int i = 0; i <= height + 1; i++) {	
		free(filteredMatrix[i]);
	}
	free(filteredMatrix);

}

void applyFilterD(int min, int max, int width, int height, unsigned char **matrix, float matrixK[3][3], int maxvalue) {
	unsigned char ** filteredMatrix = (unsigned char **)malloc((height + 2) * sizeof(unsigned char *));
	for (int i = 0; i <= height + 1; i++) {	
		filteredMatrix[i] = (unsigned char *)malloc((width + 2) * sizeof(unsigned char));
	}

	cloneMatrix(width, height, matrix, filteredMatrix);

	for (int i = min; i <= max; i++) {		
		for (int j = 1; j <= width; j++) {

			float floatResult = matrixK[0][0] * filteredMatrix[i - 1][j - 1] + matrixK[0][1] * filteredMatrix[i - 1][j] + matrixK[0][2] * filteredMatrix[i - 1][j + 1] +
							matrixK[1][0] * filteredMatrix[i][j - 1] + matrixK[1][1] * filteredMatrix[i][j] + matrixK[1][2] * filteredMatrix[i][j + 1] +
							matrixK[2][0] * filteredMatrix[i + 1][j - 1] + matrixK[2][1] * filteredMatrix[i + 1][j] + matrixK[2][2] * filteredMatrix[i + 1][j + 1];

			
			if (floatResult > (float)maxvalue) {
				floatResult = (float)maxvalue;
			}
			if (floatResult < 0.0f) {
				floatResult = 0.0f;
			}
			matrix[i][j] = (unsigned char)floatResult;
		}
		
	}

	for (int i = 0; i <= height + 1; i++) {	
		free(filteredMatrix[i]);
	}
	free(filteredMatrix);

}


int main(int argc, char * argv[]) {
	int rank;
	int nProcesses;
	FILE * f;
	FILE * out;
	char *filters[100];
	char *type;
	

	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request request;


	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	int currentArgv = 3;
	while (argv[currentArgv] != NULL) {
		filters[currentArgv - 3] = strdup(argv[currentArgv]);
		currentArgv++;
	}

	if (rank == 0) {
		unsigned char **matrix;
		unsigned char **matrixRED;
		unsigned char **matrixGREEN;
		unsigned char **matrixBLUE;
		int width, height;
		int maxvalue;
		int pnm = 0, pgm = 0;

		f = fopen(argv[1], "rb");
		out = fopen(argv[2], "wb");


		size_t bufsize = 1000;
		size_t characters;

		type = (char *)malloc(bufsize * sizeof(char));
		characters = getline(&type, &bufsize, f);

		if (strcmp(type, "P6\n") == 0) {
			pnm = 1;
		} else {
			pgm = 1;
		}

		fwrite(type, sizeof(char), characters, out);

		// line 2 - linia comentata
		char *buffer;

    	buffer = (char *)malloc(bufsize * sizeof(char));
		characters = getline(&buffer, &bufsize, f);

		fwrite(buffer, sizeof(char), characters, out);

		// line 3 - witdh - height
		fscanf(f, "%d %d\n",&width, &height);
		fprintf(out, "%d %d\n", width, height);

		// line 4 - maxvalue
		fscanf(f, "%d\n", &maxvalue);
		fprintf(out, "%d\n", maxvalue);

		if (pgm == 1) {
			matrix = malloc((height + 2) * sizeof(char *));

			for (int i = 0; i < height + 2; i++) {

				matrix[i] = malloc((width + 2) * sizeof(char));
				
				for (int j = 0; j < width + 2; j++) {
					if (i == 0 || i == height + 1 || j == 0 || j == width + 1) {
						matrix[i][j] = 0;
					} else {
						fscanf(f, "%c", &matrix[i][j]);
					}
				}
				
			}
		} else {
			matrixRED = malloc((height + 2) * sizeof(char *));
			matrixGREEN = malloc((height + 2) * sizeof(char *));
			matrixBLUE = malloc((height + 2) * sizeof(char *));

			for (int i = 0; i < height + 2; i++) {
				matrixRED[i] = malloc((width + 2) * sizeof(char));
				matrixGREEN[i] = malloc((width + 2) * sizeof(char));
				matrixBLUE[i] = malloc((width + 2) * sizeof(char));
				
				for (int j = 0; j < width + 2; j++) {
					if (i == 0 || i == height + 1 || j == 0 || j == width + 1) {
						matrixRED[i][j] = 0;
						matrixGREEN[i][j] = 0;
						matrixBLUE[i][j] = 0;
					} else {
						fscanf(f, "%c%c%c", &matrixRED[i][j], &matrixGREEN[i][j], &matrixBLUE[i][j]);
					}
				}
				
			}
		}

		for (int i = 1; i < nProcesses; i++) {
			int min = (height / nProcesses) * (i - 1);
			int max = (height / nProcesses) * i + 1;
			int heightP = max - min + 1;

			MPI_Send(&pgm, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&heightP, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&width, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		float matrixK[3][3];
		float factorK;
		for (int k = 0; k < currentArgv - 3; k++) {
			for (int i = 1; i < nProcesses; i++) {
				int min = (height / nProcesses) * (i - 1);
				int max = (height / nProcesses) * i + 1;

				if (pgm == 1) {
					for (int j = min; j <= max; j++) {
						MPI_Send(matrix[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD);
					}
				} else {
					for (int j = min; j <= max; j++) {
						MPI_Send(matrixRED[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD);
						MPI_Send(matrixGREEN[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD);
						MPI_Send(matrixBLUE[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD);
					}
				}
			}

			if (strcmp(filters[k], "smooth") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = smoothK[i][j];
					}
				}
				factorK = smoothFactor;
			}

			if (strcmp(filters[k], "blur") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = blurK[i][j];
					}
				}
				factorK = blurFactor;
			}

			if (strcmp(filters[k], "sharpen") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = sharpenK[i][j];
					}
				}
				factorK = sharpenFactor;
			}

			if (strcmp(filters[k], "mean") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = meanK[i][j];
					}
				}
				factorK = meanFactor;
			}

			if (strcmp(filters[k], "emboss") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = embossK[i][j];
					}
				}
				factorK = embossFactor;
			}

			rotatedMatrix(matrixK);
			applyFactor(factorK, matrixK);

			int min = (height / nProcesses) * (nProcesses - 1) + 1;
			int max = height;

			if (pgm == 1) {
				applyFilterD(min, max, width, height, matrix, matrixK, maxvalue);
			} else {
				applyFilterD(min, max, width, height, matrixRED, matrixK, maxvalue);
				applyFilterD(min, max, width, height, matrixGREEN, matrixK, maxvalue);
				applyFilterD(min, max, width, height, matrixBLUE, matrixK, maxvalue);
			}

			for (int i = 1; i < nProcesses; i++) {
				int min = (height / nProcesses) * (i - 1);
				int max = (height / nProcesses) * i + 1;

				if (pgm == 1) {
					for (int j = min + 1; j <= max - 1; j++) {
						MPI_Recv(matrix[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					}
				} else {
					for (int j = min + 1; j <= max - 1; j++) {
						MPI_Recv(matrixRED[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						MPI_Recv(matrixGREEN[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						MPI_Recv(matrixBLUE[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					}
				}

			}

		}

		// write to file
		for (int i = 1; i < height + 1; i++) {
			for (int j = 1; j < width + 1; j++) {
				if (pgm == 1) {
					fprintf(out, "%c", matrix[i][j]);
				} else {
					fprintf(out, "%c%c%c", matrixRED[i][j], matrixGREEN[i][j], matrixBLUE[i][j]);
				}
			}

		}
		if (pgm == 1) {
			for (int i = 0; i < height + 2; i++) {
				free(matrix[i]);
			}
			free(matrix);
		} else {
			for (int i = 0; i < height + 2; i++) {
				free(matrixRED[i]);
				free(matrixGREEN[i]);
				free(matrixBLUE[i]);
			}
			free(matrixRED);
			free(matrixGREEN);
			free(matrixBLUE);
		} 

	
	} else {

		int width, height;
		int maxvalue = 255;
		int pnm = 0, pgm = 0;

		MPI_Recv(&pgm, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&height, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&width, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		unsigned char **matrix;
		unsigned char **matrixRED;
		unsigned char **matrixGREEN;
		unsigned char **matrixBLUE;

		if (pgm == 1) {
			matrix = malloc(height * sizeof(char *));

			for (int i = 0; i < height; i++) {

				matrix[i] = malloc((width + 2) * sizeof(char));
			}

		} else {
			matrixRED = malloc(height * sizeof(char *));
			matrixGREEN = malloc(height * sizeof(char *));
			matrixBLUE = malloc(height * sizeof(char *));

			for (int i = 0; i < height; i++) {
				matrixRED[i] = malloc((width + 2) * sizeof(char));
				matrixGREEN[i] = malloc((width + 2) * sizeof(char));
				matrixBLUE[i] = malloc((width + 2) * sizeof(char));
			}
		}

		float matrixK[3][3];
		float factorK;
		for (int i = 0; i < currentArgv - 3; i++) {

			if (pgm == 1) {
				for (int j = 0; j < height; j++) {
					MPI_Recv(matrix[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
			} else {
				for (int j = 0; j < height; j++) {
					MPI_Recv(matrixRED[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					MPI_Recv(matrixGREEN[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					MPI_Recv(matrixBLUE[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
			}

			if (strcmp(filters[i], "smooth") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = smoothK[i][j];
					}
				}
				factorK = smoothFactor;
			}

			if (strcmp(filters[i], "blur") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = blurK[i][j];
					}
				}
				factorK = blurFactor;
			}

			if (strcmp(filters[i], "sharpen") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = sharpenK[i][j];
					}
				}
				factorK = sharpenFactor;
			}

			if (strcmp(filters[i], "mean") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = meanK[i][j];
					}
				}
				factorK = meanFactor;
			}

			if (strcmp(filters[i], "emboss") == 0) {
				
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						matrixK[i][j] = embossK[i][j];
					}
				}
				factorK = embossFactor;
			}

			rotatedMatrix(matrixK);
			applyFactor(factorK, matrixK);

			if (pgm == 1) {
				applyFilter(width, height - 2, matrix, matrixK, maxvalue);
				for (int j = 1; j < height - 1; j++) {
					MPI_Send(matrix[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

				}
			} else {
				applyFilter(width, height - 2, matrixRED, matrixK, maxvalue);
				applyFilter(width, height - 2, matrixGREEN, matrixK, maxvalue);
				applyFilter(width, height - 2, matrixBLUE, matrixK, maxvalue);
				for (int j = 1; j < height - 1; j++) {
					MPI_Send(matrixRED[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
					MPI_Send(matrixGREEN[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
					MPI_Send(matrixBLUE[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
				}
			}
			
		}

		if (pgm == 1) {
			for (int i = 0; i < height; i++) {
				free(matrix[i]);
			}
			free(matrix);
		} else {
			for (int i = 0; i < height; i++) {
				free(matrixRED[i]);
				free(matrixGREEN[i]);
				free(matrixBLUE[i]);
			}
			free(matrixRED);
			free(matrixGREEN);
			free(matrixBLUE);
		} 
	}
	
	MPI_Finalize();
	return 0;
}