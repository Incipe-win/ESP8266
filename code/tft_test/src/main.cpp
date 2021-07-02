#include <TFT_eSPI.h>

void setup(void) {
  TFT_eSPI tft = TFT_eSPI();
  TFT_eSprite clk = TFT_eSprite(&tft);
  clk.init(ST7735_BLACKTAB);
  clk.setRotation(1);
  clk.fillScreen(TFT_BLACK);
  clk.fillRect(0, 0, 50, 50, TFT_RED);
}

void loop() {}
