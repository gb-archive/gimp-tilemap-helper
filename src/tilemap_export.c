//
// tilemap_export.c
//

#include <stdio.h>
#include <string.h>

#include "tilemap_export.h"


 #define CALC_REM_LEN() if (len <= max_len) len_rem = max_len - len; else len_rem = 0;

// Returns string length
// If the string to write exceeds max_len then it will get cropped and return an error
uint32_t tilemap_export_c_source_to_string(char * p_dest_str, uint32_t max_len,
                                           char * p_prefix_str,
                                           tile_map_data * p_map, tile_set_data * p_tile_set) {

    uint32_t   len, len_rem;
    uint32_t   idx;

    len = 0;

    if(p_dest_str == NULL)
        return (0); // return zero length for string

    CALC_REM_LEN();
    len += (uint32_t)snprintf((p_dest_str + len), len_rem,
            "\n"
            "// Tilemap Source File \n"
            "// This file generated by: Gimp Tilemap Helper Plugin\n"
            "\n"
            "#define %s_WIDTH_TILES   %8d\n"
            "#define %s_HEIGHT_TILES  %8d\n"
            "#define %s_WIDTH_PIXELS  %8d\n"
            "#define %s_HEIGHT_PIXELS %8d\n"
            "#define %s_ENTRIES       %8d\n"
            "#define %s_TILE_WIDTH    %8d\n"
            "#define %s_TILE_HEIGHT   %8d\n"
            "\n"
            "const %s %s[] = \n"
            "{\n",
            p_prefix_str, p_map->width_in_tiles,
            p_prefix_str, p_map->height_in_tiles,
            p_prefix_str, p_map->map_width,
            p_prefix_str, p_map->map_height,
            p_prefix_str, p_map->size,
            p_prefix_str, p_map->tile_width,
            p_prefix_str, p_map->tile_height,
            (p_tile_set->tile_count <= 255) ? "unsigned char" : "unsigned int", p_prefix_str
            );

    // Write all the Tile Map data to a file
    for (idx = 0; idx < p_map->size; idx++) {

            CALC_REM_LEN();
            len += snprintf((p_dest_str + len), len_rem, "%3d,", p_map->tile_id_list[idx]);

        if (idx && (((idx+1) % 16) == 0)) {
            CALC_REM_LEN();
            len += snprintf((p_dest_str + len), len_rem, "\n"); // Line break every 8 tiles
        }

        if (idx && (((idx+1) % 64) == 0)) {
            CALC_REM_LEN();
            len += snprintf((p_dest_str + len), len_rem, "\n"); // An extra line break every 64 tiles
        }
    }

    // Close the array
    CALC_REM_LEN();
    len += snprintf((p_dest_str + len), len_rem, "};\n");



    // If the flip x/y option was enabled, then write out the per-map-entry attribute data
    if (p_map->search_mask) {

        CALC_REM_LEN();
        len += (uint32_t)snprintf((p_dest_str + len), len_rem,
                "\n\n\nconst unsigned int %s_attribs[] = \n"
                "{\n",
                p_prefix_str
                );

        // Write all the Tile Map Attrib data to a file
        for (idx = 0; idx < p_map->size; idx++) {

                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "%4x,", p_map->tile_attribs_list[idx]);

            if (idx && (((idx+1) % 16) == 0)) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "\n"); // Line break every 8 tiles
            }

            if (idx && (((idx+1) % 64) == 0)) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "\n"); // An extra line break every 64 tiles
            }
        }

        // Close the array
        CALC_REM_LEN();
        len += snprintf((p_dest_str + len), len_rem, "};\n");

    }

    return (len);
}




// Returns string length
// If the string to write exceeds max_len then it will get cropped and return an error
uint32_t tilemap_export_asm_rgbds_source_to_string(char * p_dest_str, uint32_t max_len,
                                                   char * p_prefix_str,
                                                   tile_map_data * p_map, tile_set_data * p_tile_set) {

    uint32_t   len, len_rem;
    uint32_t   idx;

    len = 0;

    if(p_dest_str == NULL)
        return (0); // return zero length for string

    // Header
    CALC_REM_LEN();
    len += (uint32_t)snprintf((p_dest_str + len), len_rem,
            ";\n"
            "; Tilemap Source File \n"
            "; This file generated by: Gimp Tilemap Helper Plugin\n"
            "\n"
            "%sWidthTiles  EQU %8d\n"
            "%sHeightTiles EQU %8d\n"
            "%sWidthPixels EQU %8d\n"
            "%sWidthPixels EQU %8d\n"
            "%sEntries     EQU %8d\n"
            "%sTileWidth   EQU %8d\n"
            "%sTileWidth   EQU %8d\n\n"
            "SECTION \"section\", HOME\n\n"
            "%sEntries::",
            p_prefix_str, p_map->width_in_tiles,
            p_prefix_str, p_map->height_in_tiles,
            p_prefix_str, p_map->map_width,
            p_prefix_str, p_map->map_height,
            p_prefix_str, p_map->size,
            p_prefix_str, p_map->tile_width,
            p_prefix_str, p_map->tile_height,
            p_prefix_str
            );

    // Write all the Tile Map data/entries to buffer

    // Different text rendering for 8 bits versus 16 bits
    if (p_tile_set->tile_count <= 255) {

        for (idx = 0; idx < p_map->size; idx++) {
            // Line break every 8 tiles, select var type based on size
            if ((idx % 16) == 0) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "\nDB ");
            }

            // Print the entry (BYTE), only trailing commas when it's not the last byte of the line
            if (((idx+1) % 16) != 0) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "$%02x,", p_map->tile_id_list[idx]);
            } else {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "$%02x", p_map->tile_id_list[idx]);
            }

            // An extra line break every 64 tiles
            if (idx && (((idx+1) % 64) == 0)) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "\n");
            }
        }
    } else {
        // Write all the Tile Map data to a file
        for (idx = 0; idx < p_map->size; idx++) {
            // Line break every 8 tiles, select var type based on size
            if ((idx % 8) == 0) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "\nDW ");
            }

            // Print the entry (WORD), only trailing commas when it's not the last byte of the line
            if (((idx+1) % 8) != 0) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "$%04x,", p_map->tile_id_list[idx]);
            } else {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "$%04x", p_map->tile_id_list[idx]);
            }

            // An extra line break every 64 tiles
            if (idx && (((idx+1) % 64) == 0)) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "\n");
            }
        }
    }

    // Finish Map Entries
    CALC_REM_LEN();
    len += snprintf((p_dest_str + len), len_rem, "\n\n");



    // If the flip x/y option was enabled, then write out the per-map-entry attribute data
    if (p_map->search_mask) {

        CALC_REM_LEN();
        len += (uint32_t)snprintf((p_dest_str + len), len_rem,
                "\n\n\n%sAttribs::",
                p_prefix_str);

        // Write all the Tile Map data to a file
        for (idx = 0; idx < p_map->size; idx++) {
            // Line break every 8 tiles, select var type based on size
            if ((idx % 8) == 0) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "\nDW ");
            }

            // Print the entry (WORD), only trailing commas when it's not the last byte of the line
            if (((idx+1) % 8) != 0) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "$%04x,", p_map->tile_attribs_list[idx]);
            } else {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "$%04x", p_map->tile_attribs_list[idx]);
            }

            // An extra line break every 64 tiles
            if (idx && (((idx+1) % 64) == 0)) {
                CALC_REM_LEN();
                len += snprintf((p_dest_str + len), len_rem, "\n");
            }
        }


        // Finish Attribs
        CALC_REM_LEN();
        len += snprintf((p_dest_str + len), len_rem, "\n");


    }

    return (len);
}
