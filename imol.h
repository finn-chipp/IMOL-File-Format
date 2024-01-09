/* IMOL -- A file format for 3D models
 * Copyright (C) 2024 Finn Chipp
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h> // For consistent sizings in the file so that the same file will be compatible with different platforms' implementations
#include <stdlib.h> // For memory allocation
#include <stdio.h> // For file i/o

#pragma pack(push, 1) // No padding for the struct to make the format easier to implement in different languages and such. Also to try to keep it consisten across platforms (so that the same file can be used on different platforms)

struct igre_model_listing { // Struct for objects that are to be read into from IMOL files
	uint64_t no_models; // Number of models within the listing

	struct igre_model {
		uint64_t name_size, // Size of model name in characters (including null terminator)
				 no_polygons; // Amount of polygons in model

		uint8_t *name; // Name of model

		struct igre_polygon {
			uint8_t fill[3]; // Fill-colour of the polygon (r, g, b)
			uint64_t vertexes[3][3]; // Vertexes of the polygon ((ax, ay, az), (bx, by, bz), (cx, cy, cz))
		} *polygons; // Array of the polygons
	} *models; // Array of the models
};

#pragma pack(pop)

enum igre_imol_error { // Error codes for load/store function return-values
	IGRE_OPERATION_SUCCESS,
	IGRE_FILE_OPEN_ERROR,
	IGRE_FILE_READ_ERROR,
	IGRE_FILE_WRITE_ERROR,
	IGRE_MEMORY_ALLOCATION_ERROR
};

enum igre_imol_error igre_load_imol(char *fileName, struct igre_model_listing *modelListing) { // Load model listing from file
	FILE *f;

	if((f = fopen(fileName, "rb")) == NULL) // Open file for reading
		return IGRE_FILE_OPEN_ERROR;

	if(!fread(&modelListing->no_models, sizeof(uint64_t), 1, f)) { // Read the amount of models in the file
		fclose(f);
		return IGRE_FILE_READ_ERROR;
	}

	if((modelListing->models = calloc(modelListing->no_models, sizeof(struct igre_model))) == NULL) { // Allocate the memory for the listing
		fclose(f);
		return IGRE_MEMORY_ALLOCATION_ERROR;
	}

	for(uint64_t i = 0; i < modelListing->no_models; i++) { // For each of the models
		if(!(fread(&modelListing->models[i].name_size, sizeof(uint64_t), 1, f) && // Load the size of its name from the file
		     fread(&modelListing->models[i].no_polygons, sizeof(uint64_t), 1, f))) { // Load the number of its polygons from the file
			fclose(f);
			return IGRE_FILE_READ_ERROR;
	    }

		if((modelListing->models[i].name = calloc(modelListing->models[i].name_size, sizeof(uint8_t))) == NULL) { // Allocate memory for its name
			fclose(f);
			return IGRE_MEMORY_ALLOCATION_ERROR;
		}

		if((modelListing->models[i].polygons = calloc(modelListing->models[i].no_polygons, sizeof(struct igre_polygon))) == NULL) { // Allocate memory for its polygons
			fclose(f);
			return IGRE_MEMORY_ALLOCATION_ERROR;
		}

		if(!(fread(modelListing->models[i].name, sizeof(uint8_t), modelListing->models[i].name_size, f) && // Load its name from the file
		     fread(modelListing->models[i].polygons, sizeof(struct igre_polygon), modelListing->models[i].no_polygons, f) == modelListing->models[i].no_polygons)) { // Load its polygons from the file
			fclose(f);
			return IGRE_FILE_READ_ERROR;
		}
	}

	// Done:

	fclose(f);
	return IGRE_OPERATION_SUCCESS;
}

enum igre_imol_error igre_store_imol(char *fileName, struct igre_model_listing *modelListing) { // Store a listing within an IMOL file
	FILE *f;

	if((f = fopen(fileName, "wb")) == NULL) // Open the file for writing
		return IGRE_FILE_OPEN_ERROR;

	if(!fwrite(&modelListing->no_models, sizeof(uint64_t), 1, f)) { // Write the number of models to the file
		fclose(f);
		return IGRE_FILE_WRITE_ERROR;
	}

	for(uint64_t i = 0; i < modelListing->no_models; i++) { // For each of the models in the listing
		if(!(fwrite(&modelListing->models[i].name_size, sizeof(uint64_t), 1, f) && // Write the size of the model name
		     fwrite(&modelListing->models[i].no_polygons, sizeof(uint64_t), 1, f))) { // Write the number of polygons in the model
			fclose(f);
			return IGRE_FILE_WRITE_ERROR;
		}

		if(!(fwrite(modelListing->models[i].name, sizeof(uint8_t), modelListing->models[i].name_size, f) == modelListing->models[i].name_size && // Write the name of the model
		     fwrite(modelListing->models[i].polygons, sizeof(struct igre_polygon), modelListing->models[i].no_polygons, f) == modelListing->models[i].no_polygons)) { // Write the model's polygons
			fclose(f);
			return IGRE_FILE_WRITE_ERROR;
		}
	}

	// Done:

	fclose(f);
	return IGRE_OPERATION_SUCCESS;
} 

void igre_free_imol(struct igre_model_listing *modelListing) { // Free all memory allocated for model listing
	for(uint64_t i = 0; i < modelListing->no_models; i++) { // Free all model names and polygons
		free(modelListing->models[i].name);
		free(modelListing->models[i].polygons);
	}

	free(modelListing->models); // Free the listing itself
}
