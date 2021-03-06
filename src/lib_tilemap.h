// lib_tilemap.h

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "image_info.h"

#ifndef LIB_TILEMAP_HEADER
#define LIB_TILEMAP_HEADER

    #define TILES_MAX_DEFAULT 8096

    #define TILE_WIDTH_DEFAULT  8
    #define TILE_HEIGHT_DEFAULT 8

    #define TILE_ID_FOUND          1
    #define TILE_ID_NOT_FOUND     -1
    #define TILE_ID_OUT_OF_SPACE  -2
    #define TILE_ID_FAILED_ENCODE -3

    // App image bit depths/modes
    enum image_modes {
        IMG_BITDEPTH_INDEXED = 1,
        IMG_BITDEPTH_INDEXED_ALPHA = 2,
        IMG_BITDEPTH_RGB = 3,
        IMG_BITDEPTH_RGB_ALPHA = 4,
        IMG_BITDEPTH_LAST
    };

    #define TILE_FLIP_BITS_NONE 0x00
    #define TILE_FLIP_BITS_X    0x01
    #define TILE_FLIP_BITS_Y    0x02
    #define TILE_FLIP_BITS_XY   (TILE_FLIP_BITS_X | TILE_FLIP_BITS_Y)
    #define TILE_FLIP_MASK      0x03
    #define TILE_FLIP_MIN       0
    #define TILE_FLIP_MIN_FLIP  1
    #define TILE_FLIP_MAX       3

    // Tile Map Entry records
    typedef struct {
        uint32_t id; // if TILES_MAX_DEFAULT > 255, this must be larger than uint8_t
        uint16_t attribs;
    } tile_map_entry;


    // Tile Map
    typedef struct {
        uint16_t width_in_tiles;
        uint16_t height_in_tiles;
        uint16_t tile_width;
        uint16_t tile_height;
        uint16_t map_width;
        uint16_t map_height;
        uint32_t size;
        uint32_t * tile_id_list; // if TILES_MAX_DEFAULT > 255, this must be larger than uint8_t
        uint16_t * tile_attribs_list;
        uint16_t search_mask;
    } tile_map_data;


    // Individual Tile from Tile Set
    typedef struct {
        uint64_t  hash[4]; // 4 hash calcs: normal, flip-x, flip-y, flip-xy
        uint8_t   raw_bytes_per_pixel;
        uint16_t  raw_width;
        uint16_t  raw_height;
        uint32_t  raw_size_bytes;     // size in bytes // TODO
        uint32_t  encoded_size_bytes; // size in bytes
        uint32_t  map_entry_count;
        uint8_t * p_img_raw;
        uint8_t * p_img_encoded;
    } tile_data;

    // Tile Set (composed of individual tiles)
    typedef struct {
        uint8_t  tile_bytes_per_pixel; // TODO: convert me to tiles[n].raw_bytes_per_pixel, raw_width, raw_height
        uint16_t tile_width;
        uint16_t tile_height;
        uint32_t tile_size;  // size in bytes
        uint32_t tile_count;
        tile_data tiles[TILES_MAX_DEFAULT];
    } tile_set_data;


    void tilemap_recalc_invalidate(void);
    void tilemap_recalc_clear_flag(void);
    int tilemap_recalc_needed(void);

    void tilemap_search_mask_set(uint16_t);

    void           tilemap_free_resources(void);
    unsigned char  process_tiles(image_data * p_src_img);
    unsigned char  tilemap_export_process(image_data * p_src_img, int tile_width, int tile_height, int check_flip);
    int32_t        tilemap_initialize(image_data * p_src_img, int tile_width, int tile_height, uint16_t search_mask);

    tile_map_data * tilemap_get_map(void);
    tile_set_data * tilemap_get_tile_set(void);

    void         tilemap_color_data_set(color_data * p_color_data);
    color_data * tilemap_color_data_get(void);

    int32_t tilemap_get_image_of_deduped_tile_set(image_data * p_img);

#endif // LIB_TILEMAP_HEADER

