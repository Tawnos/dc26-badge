#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/rawbmp.h>

#include "virtualHAL.h"
#include <art/images.h>
#include <libstm32/rgbcolor.h>
#include <display_device.h>
#include <darknet7.h>
#include "VirtualDisplayDevice.h"

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

  cmdc0de::ErrorType et;

  cmdc0de::GUIListItemData items[3];
  cmdc0de::GUIListData DrawList((const char*)"Self Check", items, uint8_t(0),
    uint8_t(0), uint8_t(DISPLAY_WIDTH), uint8_t(DISPLAY_HEIGHT / 2), uint8_t(0), uint8_t(0));
  //DO SELF CHECK
  if ((et = Display->init(&DisplayBuffer, &Font_6x10)).ok()) {
    items[0].set(0, "OLED_INIT");
    DrawList.ItemsCount++;
    Display->setTextColor(cmdc0de::RGBColor::WHITE);
  }
  else {
    /*while (1) {
      HAL_GPIO_WritePin(SIMPLE_LED1_GPIO_Port, SIMPLE_LED1_Pin, GPIO_PIN_SET);
      HAL_Delay(200);
      HAL_GPIO_WritePin(SIMPLE_LED1_GPIO_Port, SIMPLE_LED1_Pin, GPIO_PIN_RESET);
      HAL_Delay(400);
    }*/
  }
  cmdc0de::GUI gui(Display);
  gui.drawList(&DrawList);
  Display->update();
  HAL_Delay(500);
  if (MyContacts.getMyInfo().init()) {
    items[1].set(1, "Flash INIT");
  }
  else {
    items[1].set(1, "Flash INIT FAILED!");
  }
  DrawList.ItemsCount++;
  DrawList.selectedItem++;
  gui.drawList(&DrawList);
  Display->update();
  HAL_Delay(500);
  MyContacts.getSettings().init();
  Display->fillScreen(cmdc0de::RGBColor::BLACK);
  Display->update();
  Display->drawImage(32, 16, getLogo1());
  Display->update();
  HAL_Delay(1000);
  Display->drawImage(32, 16, getLogo2());
  Display->update();
  HAL_Delay(1000);
  Display->drawImage(32, 16, getLogo3());
  Display->update();
  HAL_Delay(1000);

  return cmdc0de::ErrorType{};
}
cmdc0de::ErrorType DarkNet7::onRun() {
  cmdc0de::StateBase::ReturnStateContext rsc = getCurrentState()->run();
  Display->update();
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

static auto displayDevice = VirtualDisplayDevice(DISPLAY_WIDTH, DISPLAY_HEIGHT, START_ROT);
static auto frameBuffer = cmdc0de::DrawBuffer2D16BitColor16BitPerPixel1Buffer(&displayDevice, new uint16_t[DISPLAY_WIDTH*DISPLAY_HEIGHT]);
static auto darknet = DarkNet7(&displayDevice);

DarkNet7* DarkNet7::instance = &darknet;
bool MyApp::OnInit()
{
  displayDevice.init(&frameBuffer, &Font_6x10);
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

  darknet.run();
  return true;
}

bool MCUToMCU::send(flatbuffers::FlatBufferBuilder const& fbb) { return false; }

static MCUToMCU mcu{};

MCUToMCU& DarkNet7::getMcuToMcu() { return mcu; }
const MCUToMCU& DarkNet7::getMcuToMcu() const { return mcu; }

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