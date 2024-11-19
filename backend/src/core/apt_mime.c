// #include <string.h>

// #include <ctype.h>
// #include "mime.h"

// #define DEFAULT_MIME_TYPE "application/octet-stream"

// /**
//  * Lowercase a string
//  */
// char *strlower(char *s)
// {
//     for (char *p = s; *p != '\0'; p++) {
//         *p = tolower(*p);
//     }

//     return s;
// }

// /**
//  * Return a MIME type for a given filename
//  */
// char *mime_type_get(char *filename)
// {
//     char *ext = strrchr(filename, '.');

//     if (ext == NULL) {
//         return DEFAULT_MIME_TYPE;
//     }
    
//     ext++;

//     strlower(ext);

//     // TODO: this is O(n) and it should be O(1)

//     if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) { return "text/html"; }
//     if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0) { return "image/jpg"; }
//     if (strcmp(ext, "css") == 0) { return "text/css"; }
//     if (strcmp(ext, "js") == 0) { return "application/javascript"; }
//     if (strcmp(ext, "json") == 0) { return "application/json"; }
//     if (strcmp(ext, "txt") == 0) { return "text/plain"; }
//     if (strcmp(ext, "gif") == 0) { return "image/gif"; }
//     if (strcmp(ext, "png") == 0) { return "image/png"; }
//     if (strcmp(ext, "mp4") == 0) { return ""}

//     return DEFAULT_MIME_TYPE;
// }



#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_MIME_TYPE "application/octet-stream"

// Struct ánh xạ giữa phần mở rộng và MIME type
struct mime_map {
    char *extension;
    char *mime_type;
};

// Danh sách ánh xạ phần mở rộng và MIME type
struct mime_map mime_types[] = {
    {"html", "text/html"},
    {"htm", "text/html"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"}, 
    {"css", "text/css"},
    {"js", "application/javascript"},
    {"json", "application/json"},
    {"txt", "text/plain"},
    {"gif", "image/gif"},
    {"png", "image/png"},
    {"mp4", "video/mp4"},
    {NULL, NULL} // Kết thúc danh sách
};

// Hàm chuyển phần mở rộng về chữ thường
void strlower(char *str) {
    while (*str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

// Hàm lấy MIME type từ filename
char *mime_type_get(char *filename)
{
    char *ext = strrchr(filename, '.');
    
    if (ext == NULL) {
        return DEFAULT_MIME_TYPE;
    }
    
    ext++; // Bỏ qua dấu '.'
    strlower(ext);

    // Duyệt qua bảng ánh xạ để tìm MIME type tương ứng
    for (int i = 0; mime_types[i].extension != NULL; i++) {
        if (strcmp(ext, mime_types[i].extension) == 0) {
            return mime_types[i].mime_type;
        }
    }

    return DEFAULT_MIME_TYPE;
}
