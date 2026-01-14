#include "Arduino.h"
#include "FS.h"
#include "FFat.h"
#include "Image.h"
#include "OV2640.h"
#include "ST7789V.h"
#include "imlib.h"
#include "FFat.h"
#include "BmpImage.h"
using namespace K210;

// 定义摄像头和LCD
OV2640 cam;
ST7789V lcd(240, 320);
#define FORMAT_FFAT 0

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void find_lines(image_t *img_ts) {
  image_t *arg_img = img_ts;
Serial.println("line start");

  rectangle_t roi;
  roi.x = 0;
  roi.y = 0;
  roi.w = 320;
  roi.h = 320;


  unsigned int x_stride = 2;
  unsigned int y_stride = 1;
  uint32_t threshold = 1000;
  unsigned int theta_margin = 25;
  unsigned int rho_margin = 25;

  list_t out_;
  fb_alloc_mark();
  imlib_find_lines(&out_, arg_img, &roi, x_stride, y_stride, threshold, theta_margin, rho_margin);
  fb_alloc_free_till_mark();


  for (size_t i = 0; list_size(&out_); i++) {
    find_lines_list_lnk_data_t lnk_data;
    list_pop_front(&out_, &lnk_data);
    Serial.println("fine line ones!\n");





    Serial.println(String(lnk_data.line.x1));
    Serial.println(String(lnk_data.line.y1));
    Serial.println(String(lnk_data.line.x2));
    Serial.println(String(lnk_data.line.y2));

    // py_line_obj_t *o = m_new_obj(py_line_obj_t);
    // o->base.type = &py_line_type;
    // o->x1 = mp_obj_new_int(lnk_data.line.x1);
    // o->y1 = mp_obj_new_int(lnk_data.line.y1);
    // o->x2 = mp_obj_new_int(lnk_data.line.x2);
    // o->y2 = mp_obj_new_int(lnk_data.line.y2);
    // int x_diff = lnk_data.line.x2 - lnk_data.line.x1;
    // int y_diff = lnk_data.line.y2 - lnk_data.line.y1;
    // o->length = mp_obj_new_int(fast_roundf(fast_sqrtf((x_diff * x_diff) + (y_diff * y_diff))));
    // o->magnitude = mp_obj_new_int(lnk_data.magnitude);
    // o->theta = mp_obj_new_int(lnk_data.theta);
    // o->rho = mp_obj_new_int(lnk_data.rho);

    // objects_list->items[i] = o;
  }
Serial.println("blob end");

}



void find_blobs(image_t *img_ts) {
  image_t *arg_img = img_ts;
Serial.println("blob start");
  list_t thresholds;
  thresholds.size = 100;
  list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));

  //WORN_PRINT("list info, size:%d",thresholds.size);

  color_thresholds_list_lnk_data_t tmp_ct;
  tmp_ct.LMin = 44;
  tmp_ct.AMin = 10;
  tmp_ct.BMin = -49;
  tmp_ct.LMax = 80;
  tmp_ct.AMax = 110;
  tmp_ct.BMax = 115;
  list_push_back(&thresholds, &tmp_ct);

  bool invert = false;

  rectangle_t roi;
  roi.x = 0;
  roi.y = 0;
  roi.w = 320;
  roi.h = 320;


  unsigned int x_stride = 2;
  unsigned int y_stride = 1;

  unsigned int area_threshold = 10;
  unsigned int pixels_threshold = 10;
  bool merge = false;
  int margin = 0;
  unsigned int x_hist_bins_max = 0;
  unsigned int y_hist_bins_max = 0;

  list_t out;
  fb_alloc_mark();
  Serial.println("blob start");
  imlib_find_blobs(&out, arg_img, &roi, x_stride, y_stride, &thresholds, invert,
                   area_threshold, pixels_threshold, merge, margin,
                   NULL, NULL, NULL, NULL, x_hist_bins_max, y_hist_bins_max);
  fb_alloc_free_till_mark();

  list_free(&thresholds);

  for (size_t i = 0; list_size(&out); i++) {
    find_blobs_list_lnk_data_t lnk_data;
    list_pop_front(&out, &lnk_data);
    Serial.println("find ones!\n");

    Serial.println("find ones!\n");
    Serial.println(String(lnk_data.rect.x));
    Serial.println(String(lnk_data.rect.y));
    Serial.println(String(lnk_data.rect.w));
    Serial.println(String(lnk_data.rect.h));
  }
Serial.println("blob end");

}

void find_cricle(image_t *img_ts) {
  image_t *arg_img = img_ts;
Serial.println("circle start");
  rectangle_t roi;
  roi.x = 0;
  roi.y = 0;
  roi.w = 320;
  roi.h = 320;

  unsigned int x_stride = 2;

  unsigned int y_stride = 1;

  uint32_t threshold = 3000;
  unsigned int x_margin = 10;
  unsigned int y_margin = 10;
  unsigned int r_margin = 10;
  unsigned int r_min = 15;
  unsigned int r_max = 25;
  unsigned int r_step = 2;

  list_t out;
  fb_alloc_mark();
  imlib_find_circles(&out, arg_img, &roi, x_stride, y_stride, threshold, x_margin, y_margin, r_margin,
                     r_min, r_max, r_step);
  fb_alloc_free_till_mark();


  for (size_t i = 0; list_size(&out); i++) {
    find_circles_list_lnk_data_t lnk_data;
    list_pop_front(&out, &lnk_data);
    printf("find_cricles!\n");

    Serial.println(String(lnk_data.p.x));
    Serial.println(String(lnk_data.p.y));
    Serial.println(String(lnk_data.r));
  }
  Serial.println("circle end");
}






const char *path1 = "/24tmp_img_big.bmp";
image_t *img_ts = (image_t *)malloc(sizeof(image_t));    // = imlib_image_create(240, 240, PIXFORMAT_BINARY, 0, NULL, false);
image_t *img_ts_1 = (image_t *)malloc(sizeof(image_t));  // = imlib_image_create(240, 240, PIXFORMAT_BINARY, 0, NULL, false);

BmpImage bmp;
// 定义图像指针
void setup() {
  // put your setup code here, to run once:
  imlib_init();
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  if (!FFat.begin()) {
    Serial.println("FFat Mount Failed");
    return;
    Serial.println("FFat Mount success");
  }

  Serial.println("FFat read");
  Serial.printf("Total space: %ld\n", FFat.totalBytes());
  Serial.printf("Free space: %ld\n", FFat.freeBytes());
  listDir(FFat, "/", 0);

  File myFile = FFat.open(path1);
  if (!myFile) {
    Serial.println(String(path1) + " not found");
    return;
  }

  /* show the image */
  //  bmp.begin(myFile);
  //  BmpImage::BMP_IMAGE_PIX_FMT fmt = bmp.getPixFormat();
  /*
   *  BMP_IMAGE_NONE   = 0
   *  BMP_IMAGE_GRAY8  = 1
   *  BMP_IMAGE_RGB565 = 2
   *  BMP_IMAGE_RGB888 = 3
   */
  //  if (fmt > 0) {
  //    Serial.println(path1);
  //    Serial.println("Image format : " + String(fmt-1));
  //    Serial.println("Image width  : " + String(bmp.getWidth()));
  //    Serial.println("Image height : " + String(bmp.getHeight()));
  //  } else {
  //    Serial.println("Fail: cannot read : " + String(bmp.getHeight()));

  //  }

  K210::Image *img, *lol;
  lol = new Image();
  K210::Image::load_bmp(lol, FFat, path1);
  Serial.printf("Loaded: %ux%u format=%d bpp=%d\n", lol->width(), lol->height(), lol->format(), lol->bpp());
  //img = lol->
  //Image::to_rgb565(lol,img,1);
  //image_alloc(img_ts,lol->size());
  img = new Image(lol->width(), lol->height(), IMAGE_FORMAT_RGB565, (uint8_t *)rt_malloc_align(320 * 320 * 2, 8));
  img = lol->to_rgb565();
  //if(lol->format()!=IMAGE_FORTMAT_RGB565)&&

  //img = lol;
  Serial.printf("Loaded: %ux%u format=%u bpp=%d\n", img->width(), img->height(), img->format(), img->bpp());
  int size = img->width() * img->height() * img->bpp();
  image_alloc(img_ts_1,size);
  int r = img->Image::as_imlib_image(img_ts_1);
  img_ts = img_ts_1;
  Serial.println(r);
  if (r != 0) {
    Serial.printf("convertion failed\n");
  } else {
    Serial.printf("convertion passed\n");
  }
  Serial.print("IMAGE_FORMAT_GRAYSCALE ");
  Serial.println(IMAGE_FORMAT_GRAYSCALE);

  Serial.print("IMAGE_FORMAT_RGB565 ");
  Serial.println(IMAGE_FORMAT_RGB565);

  Serial.print("img->format() ");
  Serial.println(img->format());

  // Serial.print("PIXFORMAT_RGB565 ");
  // Serial.println(PIXFORMAT_RGB565);
  Serial.printf("img_ts:%ux%u pixfmt=%d data=%p\n", img_ts->w, img_ts->h, img_ts->pixfmt, img_ts->data);
  Serial.print("image_size(img_ts) ");
  Serial.println(image_size(img_ts));


  if ((IMAGE_FORMAT_GRAYSCALE != img->format())) { Serial.println("RROR-GG"); }
  if ((IMAGE_FORMAT_RGB565 != img->format())) { Serial.println("RROR-RR"); }
  if (img) {

    Serial.println("image read");
    //  img_ts->w = img->width();  ///
    //  img_ts->h = img->height();//
    //  img_ts->pixels = img->pixel();//img->pixel();//
    //  img_ts->data = img->pixel();//
    //    img_ts->raw=0x01;

    // img_ts->pixfmt = img->bpp();//
  Serial.print("img_ts->w  ");
    Serial.println(img_ts->w);
    Serial.print("img_ts->w = img->width() ");
    Serial.println(img_ts->w = img->width());
    Serial.print("img_ts->w = img->width() ");
    Serial.println(img_ts->w);

    Serial.println();
  } else {
    Serial.println("image notread");
  }




  //imlib_load_image(img_ts,"/565.bmp");
  //imlib_draw_line(img_ts, 10, 10, 10, 100, COLOR_R8_G8_B8_TO_RGB888(0xff, 0x00, 0x00), 4);
  //imlib_draw_line(img_ts, 20, 10, 20, 100, COLOR_R8_G8_B8_TO_RGB888(0x00, 0xff, 0x00), 4);
  //imlib_draw_line(img_ts, 30, 10, 30, 100, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 4);
  // imlib_draw_rectangle(img_ts, 10, 120, 14, 100, COLOR_R8_G8_B8_TO_RGB888(0xff, 0x00, 0x00), 2, 0);
  //imlib_draw_rectangle(img_ts, 26, 120, 14, 100, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 1, 1);
  //imlib_draw_circle(img_ts, 70, 30, 20, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 2, false);
  // imlib_draw_circle(img_ts, 70, 100, 20, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 2, true);
  // imlib_draw_string(img_ts, 70, 150, "nihao",     COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 3.0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
  find_cricle(img_ts);
  find_lines(img_ts);
  find_blobs(img_ts);

  image_t *test_img = (image_t *)malloc(sizeof(image_t));
  if (test_img) {
    test_img->w = 320;
    test_img->h = 240;
    test_img->size = 320 * 240 * 2;
    test_img->data = NULL;
  }                                              // Verify framebuffer was updated
  if (test_img->w == 320 && test_img->h == 240)  //&&test_img->pixfmt == K210::PIXFORMAT_RGB565)
  {

    Serial.print("test_img->w ");
    Serial.println(test_img->w);
    Serial.print("test_img->h ");
    Serial.println(test_img->h);
    Serial.println("VERIFY");

  } else {
    Serial.println("NOT-VERIFY");
  }

  Serial.println("D");
}

void loop() {
  // put your main code here, to run repeatedly:
}
