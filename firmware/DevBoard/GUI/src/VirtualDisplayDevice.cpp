
#include "VirtualDisplayDevice.h"
#include <wx/wx.h>
/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void BasicDrawPane::paintEvent(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 *
 * In most cases, this will not be needed at all; simply handling
 * paint events and calling Refresh() when a refresh is needed
 * will do the job.
 */
void BasicDrawPane::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void BasicDrawPane::render(wxDC &dc)
{
    dc.DrawBitmap(bitmap, 0, 0);
}

class VirtualDisplayDevice : public cmdc0de::DisplayDevice {
private:

public:
  VirtualDisplayDevice(uint16_t w, uint16_t h, cmdc0de::RotationType r) : DisplayDevice(w, h, r) {}
  virtual ~VirtualDisplayDevice() override { }

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
  virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, cmdc0de::RGBColor color) override
  {
  }

  // Inherited via DisplayDevice
  virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const cmdc0de::RGBColor& textColor, const cmdc0de::RGBColor& bgColor, uint8_t size, bool lineWrap, uint8_t charsToRender)
  {
    return uint32_t();
  }

  // Inherited via DisplayDevice
  virtual void swap() override
  {
  }

  // Inherited via DisplayDevice
  virtual cmdc0de::ErrorType init(cmdc0de::PixelFormat pf, const FontDef_t* defaultFont, cmdc0de::FrameBuf*) override
  {
    return cmdc0de::ErrorType();
  }
  virtual void reset()
  {
  }
};
