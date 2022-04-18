#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/rawbmp.h>

#include "virtualHAL.h"
#include <art/images.h>
#include <libstm32/rgbcolor.h>
#include <display_device.h>
#include <darknet7.h>

class wxCustomButton : public wxWindow
{

  bool pressedDown;
  wxString text;

  static const int buttonWidth = 50;
  static const int buttonHeight = 50;

public:
  wxCustomButton(wxFrame* parent, wxString text, Button b) :
    wxWindow(parent, wxID_ANY),
    text(text),
    pressedDown(false)
  {
    SetMinSize(wxSize(buttonWidth, buttonHeight));
  }

  void paintEvent(wxPaintEvent& evt);
  void paintNow();

  void render(wxDC& dc);

  // some useful events
  void mouseMoved(wxMouseEvent& event);
  void mouseDown(wxMouseEvent& event);
  void mouseWheelMoved(wxMouseEvent& event);
  void mouseReleased(wxMouseEvent& event);
  void rightClick(wxMouseEvent& event);
  void mouseLeftWindow(wxMouseEvent& event);
  void keyPressed(wxKeyEvent& event);
  void keyReleased(wxKeyEvent& event);

  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxCustomButton, wxPanel)

EVT_MOTION(wxCustomButton::mouseMoved)
EVT_LEFT_DOWN(wxCustomButton::mouseDown)
EVT_LEFT_UP(wxCustomButton::mouseReleased)
EVT_RIGHT_DOWN(wxCustomButton::rightClick)
EVT_LEAVE_WINDOW(wxCustomButton::mouseLeftWindow)
EVT_KEY_DOWN(wxCustomButton::keyPressed)
EVT_KEY_UP(wxCustomButton::keyReleased)
EVT_MOUSEWHEEL(wxCustomButton::mouseWheelMoved)

// catch paint events
EVT_PAINT(wxCustomButton::paintEvent)

END_EVENT_TABLE()

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

  void wxCustomButton::paintEvent(wxPaintEvent& evt)
{
  // depending on your system you may need to look at double-buffered dcs
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
 */
void wxCustomButton::paintNow()
{
  // depending on your system you may need to look at double-buffered dcs
  wxClientDC dc(this);
  render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void wxCustomButton::render(wxDC& dc)
{
  if (pressedDown)
    dc.SetBrush(*wxRED_BRUSH);
  else
    dc.SetBrush(*wxGREY_BRUSH);

  dc.DrawRectangle(0, 0, buttonWidth, buttonHeight);
  dc.DrawText(text, 5, 5);
}

void wxCustomButton::mouseDown(wxMouseEvent& event)
{
  pressedDown = true;
  //ButtonState |= button;
  paintNow();
}
void wxCustomButton::mouseReleased(wxMouseEvent& event)
{
  pressedDown = false;
  //ButtonState -= button;
  paintNow();
}
void wxCustomButton::mouseLeftWindow(wxMouseEvent& event)
{
  if (pressedDown)
  {
    pressedDown = false;
    //ButtonState -= button;
    paintNow();
  }
}

// currently unused events
void wxCustomButton::mouseMoved(wxMouseEvent& event) {}
void wxCustomButton::mouseWheelMoved(wxMouseEvent& event) {}
void wxCustomButton::rightClick(wxMouseEvent& event) {}
void wxCustomButton::keyPressed(wxKeyEvent& event) {}
void wxCustomButton::keyReleased(wxKeyEvent& event) {}

class VirtualDisplayDevice : public cmdc0de::DisplayDevice {
public:
  VirtualDisplayDevice(uint16_t w, uint16_t h, cmdc0de::Rotation r) : DisplayDevice(w, h, r) {}
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
  virtual const FontDef_t* getFont() override
  {
    return nullptr;
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
};

class VirtualStateBase : public cmdc0de::StateBase {
  // Inherited via StateBase
  virtual cmdc0de::ErrorType onInit() override
  {
    return cmdc0de::ErrorType();
  }
  virtual ReturnStateContext onRun() override
  {
    return ReturnStateContext(this);
  }
  virtual cmdc0de::ErrorType onShutdown() override
  {
    return cmdc0de::ErrorType();
  }
};

cmdc0de::StateBase::ReturnStateContext cmdc0de::StateBase::run() {
  ++TimesRunCalledAllTime;
  ReturnStateContext sr(this);
  if (!hasBeenInitialized()) {
    TimesRunCalledSinceLastReset = 0;
    ErrorType et = init();
    if (!et.ok()) {
      sr.NextMenuToRun = 0;
      sr.Err = et;
    }
  }
  else {
    ++TimesRunCalledSinceLastReset;
    sr = onRun();
    if (sr.NextMenuToRun != this) {
      shutdown();
    }
  }
  return sr;
}

cmdc0de::ErrorType cmdc0de::StateBase::init() {
  ErrorType et = onInit();
  if (et.ok()) {
    setState(INIT_BIT);
    StateStartTime = HAL_GetTick();
  }
  return et;
}

cmdc0de::ErrorType cmdc0de::StateBase::shutdown() {
  ErrorType et = onShutdown();
  clearState(INIT_BIT);
  StateStartTime = 0;
  return et;
}

cmdc0de::ErrorType DarkNet7::onInit() {
  return cmdc0de::ErrorType{};
}
cmdc0de::ErrorType DarkNet7::onRun() {
  cmdc0de::StateBase::ReturnStateContext rsc = getCurrentState()->run();
	Display->swap();
	//handleLEDS();

	if (rsc.Err.ok()) {
		if (getCurrentState() != rsc.NextMenuToRun) {
			//on state switches reset keyboard and give a 1 second pause on reading from keyboard.
			MyButtons.reset();
			setCurrentState(rsc.NextMenuToRun);
		}
		else {
			/*if (getCurrentState() != DarkNet7::instance->getGameOfLifeState()
				&& (HAL_GetTick() - MyButtons.lastTickButtonPushed()) > (DarkNet7::instance->getContacts().getSettings().getScreenSaverTime() * 1000 * 60)) {
				setCurrentState(DarkNet7::instance->getGameOfLifeState());
			}*/
		}
	}
	else {
		//setCurrentState(StateCollection::getDisplayMessageState(
		//		StateCollection::getDisplayMenuState(), (const char *)"Run State Error....", uint16_t (2000)));
	}

  return cmdc0de::ErrorType{};
}
class BasicDrawPane : public wxPanel
{
private:
  wxBitmap bitmap;

public:
  BasicDrawPane(wxFrame* parent, wxBitmap bitmap)
    : wxPanel(parent), bitmap(bitmap) {}

  void paintEvent(wxPaintEvent& evt);
  void paintNow();

  void render(wxDC& dc);

  DECLARE_EVENT_TABLE()
};

class MyApp : public wxApp
{
  bool OnInit();

  wxFrame* frame;
  wxCustomButton* btnUp;
  wxCustomButton* btnDown;
  wxCustomButton* btnLeft;
  wxCustomButton* btnRight;
  wxCustomButton* btnMid;
  wxCustomButton* btnFire;
  BasicDrawPane* drawPane;

public:
};

IMPLEMENT_APP(MyApp)

enum COLOR { // 2/2/2
  BLACK = 0,
  RED_MASK = 0x30,
  GREEN_MASK = 0xC,
  BLUE_MASK = 0x3,
  WHITE = 0x3F,
  BITS_PER_PIXEL = 2
};

static auto darknet = DarkNet7{
  new VirtualDisplayDevice(DISPLAY_WIDTH, DISPLAY_HEIGHT, START_ROT)
};
DarkNet7* DarkNet7::instance = &darknet;
bool MyApp::OnInit()
{
  darknet.init();
  wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
  frame = new wxFrame((wxFrame*)NULL, -1, wxT("Darknet-7"), wxPoint(50, 50), wxSize(1024, 768));

  btnUp = new wxCustomButton(frame, wxT("Up"), Button::Up);
  btnDown = new wxCustomButton(frame, wxT("Down"), Button::Down);
  btnLeft = new wxCustomButton(frame, wxT("Left"), Button::Left);
  btnRight = new wxCustomButton(frame, wxT("Right"), Button::Right);

  leftSizer->Add(btnUp, 0, wxALL, 5);
  leftSizer->Add(btnDown, 0, wxALL, 5);
  leftSizer->Add(btnLeft, 0, wxALL, 5);
  leftSizer->Add(btnRight, 0, wxALL, 5);

  hSizer->Add(leftSizer);

  auto titsLogo = getLogo4();
  auto logoBitmap = wxBitmap{ (int)titsLogo.width, (int)titsLogo.height, 24 };
  auto nativePixels = wxNativePixelData{ logoBitmap };
  auto pixelIterator = wxNativePixelData::Iterator{ nativePixels };

  static const uint8_t colorValues[4] = { 0,85,170,255 };
  auto pixels = (uint16_t*)&titsLogo.pixel_data[0];
  int curPixelDataLoc = 0;
  for (int y = 0; y < (int)titsLogo.width; ++y)
  {
    wxNativePixelData::Iterator rowStart = pixelIterator;
    for (int x = 0; x < (int)titsLogo.height; ++x, ++pixelIterator)
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

  drawPane = new BasicDrawPane((wxFrame*)frame, logoBitmap);
  hSizer->Add(drawPane, 1, wxEXPAND);


  btnMid = new wxCustomButton(frame, wxT("Mid"), Button::Mid);
  btnFire = new wxCustomButton(frame, wxT("Fire"), Button::Fire);
  rightSizer->Add(btnMid, 0, wxALL, 5);
  rightSizer->Add(btnFire, 0, wxALL, 5);

  hSizer->Add(rightSizer);

  frame->SetSizer(hSizer);
  frame->SetAutoLayout(true);

  frame->Show();
  return true;
}

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)

EVT_PAINT(BasicDrawPane::paintEvent)

END_EVENT_TABLE()


/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

  void BasicDrawPane::paintEvent(wxPaintEvent& evt)
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
void BasicDrawPane::render(wxDC& dc)
{
  dc.DrawBitmap(bitmap, 0, 0);
}

bool MCUToMCU::send(flatbuffers::FlatBufferBuilder const& fbb) { return false; }

static MCUToMCU mcu{};

MCUToMCU& DarkNet7::getMcuToMcu() { return mcu; }
const MCUToMCU& DarkNet7::getMcuToMcu() const { return mcu; }

void cmdc0de::FrameBuf::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {}
bool cmdc0de::FrameBuf::writeNData(const uint8_t* data, int nbytes) { return false; }
uint32_t cmdc0de::StateBase::timeInState() { return 0; }

void ButtonInfo::processButtons() {}

cmdc0de::ErrorType cmdc0de::App::init() {
  return onInit();
}

cmdc0de::ErrorType cmdc0de::App::run() {
  //LastRunTime = HAL_GetTick();
  ErrorType et = onRun();
  //LastRunPerformance = HAL_GetTick() - LastRunTime;
  return et;
}