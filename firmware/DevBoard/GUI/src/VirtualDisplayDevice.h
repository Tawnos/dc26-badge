#pragma once

#include <darknet7.h>
#include <display_device.h>
class BasicDrawPane : public wxPanel
{
private:
    wxBitmap bitmap;

public:
    BasicDrawPane(wxFrame *parent, wxBitmap bitmap)
        : wxPanel(parent), bitmap(bitmap) {}

    void paintEvent(wxPaintEvent &evt)
    {
        wxPaintDC dc(this);
        render(dc);
    }
    void paintNow()
    {
        wxClientDC dc(this);
        render(dc);
    }

    void render(wxDC &dc)
    {
        dc.DrawBitmap(bitmap, 0, 0);
    }

    DECLARE_EVENT_TABLE()
};

class VirtualDisplayDevice : public cmdc0de::DisplayDevice, wxPanel
{
public:
  VirtualDisplayDevice(uint16_t w, uint16_t h, cmdc0de::RotationType r) 
    : DisplayDevice(w, h, r) {}
  virtual ~VirtualDisplayDevice() override { }

private:
  uint16_t imageBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
  cmdc0de::DrawBuffer2D16BitColor16BitPerPixel1Buffer fb{this, imageBuffer};
  wxBitmap bitmap;

public:

  // Inherited via DisplayDevice
  virtual bool drawPixel(uint16_t x0, uint16_t y0, const cmdc0de::RGBColor& color) override
  {
    return false;
  }
  virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const cmdc0de::RGBColor& color) override
  {
  }
  virtual void drawRec(int16_t x, int16_t y, int16_t w, int16_t h, const cmdc0de::RGBColor& color) override
  {
  }
  virtual void fillScreen(const cmdc0de::RGBColor& color) override
  {
  }
  virtual void drawImage(int16_t x, int16_t y, const cmdc0de::DCImage& dcImage) override
  {
    auto bitmap = wxBitmap{ (int)dcImage.width, (int)dcImage.height, 24 };
    auto nativePixels = wxNativePixelData{ bitmap };
    auto pixelIterator = wxNativePixelData::Iterator{ nativePixels };

    auto pixels = (uint16_t*)&dcImage.pixel_data[0];
    int curPixelDataLoc = 0;
    for (int y = 0; y < (int)dcImage.width; ++y)
    {
      wxNativePixelData::Iterator rowStart = pixelIterator;
      for (int x = 0; x < (int)dcImage.height; ++x, ++pixelIterator)
      {
        auto packedColor = pixels[curPixelDataLoc++];
        uint32_t rc = 8 * ((packedColor & 0b1111100000000000) >> 11);
        uint32_t gc = 4 * ((packedColor & 0b0000011111100000) >> 5);
        uint32_t bc = 8 * (packedColor & 0b0000000000011111);

        pixelIterator.Red() = rc;
        pixelIterator.Green() = gc;
        pixelIterator.Blue() = bc;
      }
      pixelIterator = rowStart;
      pixelIterator.OffsetY(nativePixels, 1);
    }
    //dc.DrawBitmap(bitmap, 0, 0);
  }
  virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt) override
  {
    return uint32_t();
  }
  virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const cmdc0de::RGBColor& textColor) override
  {
    return uint32_t();
  }
  virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const cmdc0de::RGBColor& textColor, const cmdc0de::RGBColor& bgColor, uint8_t size, bool lineWrap) override
  {
    return uint32_t();
  }
  virtual uint32_t drawStringOnLine(uint8_t line, const char* msg) override
  {
    return uint32_t();
  }
  virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w) override
  {
  }
  virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, const cmdc0de::RGBColor& color) override
  {
  }

  // Inherited via DisplayDevice
  virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const cmdc0de::RGBColor& textColor, const cmdc0de::RGBColor& bgColor, uint8_t size, bool lineWrap, uint8_t charsToRender)
  {
    return uint32_t();
  }

  // Inherited via DisplayDevice
  virtual void update() override
  {
  }

  // Inherited via DisplayDevice
  virtual cmdc0de::ErrorType onInit() override
  {
    return cmdc0de::ErrorType();
  }

  virtual void reset()
  {
  }

  void paintEvent(wxPaintEvent &evt)
  {
      wxPaintDC dc(this);
      render(dc);
  }
  void paintNow()
  {
      wxClientDC dc(this);
      render(dc);
  }

  void render(wxDC &dc)
  {
      dc.DrawBitmap(bitmap, 0, 0);
  }

  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)

EVT_PAINT(BasicDrawPane::paintEvent)

END_EVENT_TABLE()

BEGIN_EVENT_TABLE(VirtualDisplayDevice, wxPanel)

EVT_PAINT(VirtualDisplayDevice::paintEvent)

END_EVENT_TABLE()