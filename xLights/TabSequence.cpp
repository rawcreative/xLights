#include <wx/utils.h> //check keyboard state -DJ
#include <wx/tokenzr.h>
#include <wx/clipbrd.h>
#include "xLightsMain.h"
#include "heartbeat.h"


//#define WANT_FASTER_GRID(grid)  MakeGridFaster grid##_faster(grid) //experimental; comment out this line for slower grid updates -DJ

#ifdef WANT_FASTER_GRID
 class MakeGridFaster //scoped wrapper to turn grid updates off+on
 {
 private:
    wxGrid* grid_ptr;
 public:
    MakeGridFaster(wxGrid* grid): grid_ptr(grid)
    {
        if (grid_ptr) grid_ptr->BeginBatch(); //defer repaints until finished updating
    }
    ~MakeGridFaster(void)
    {
        if (grid_ptr) grid_ptr->EndBatch(); //resume updates when wrapper goes out of scope
    }
 };
#else
 #define WANT_FASTER_GRID(ignore) //works as before
#endif // FASTER_GRID


void xLightsFrame::CreateDefaultEffectsXml()
{
    wxXmlNode* root = new wxXmlNode( wxXML_ELEMENT_NODE, "xrgb" );
    EffectsXml.SetRoot( root );
}


wxXmlNode* xLightsFrame::GetModelNode(const wxString& name)
{
    wxXmlNode* e;
    for(e=ModelsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "model")
        {
            if (name == e->GetAttribute("name")) return e;
        }
    }
    return NULL;
}

void xLightsFrame::OnButton_PlayAllClick(wxCommandEvent& event)
{
    PlayRgbSequence();
}

void xLightsFrame::PlayRgbSequence()
{
    if (SeqPlayerState == PLAYING_SEQ
            || SeqPlayerState == PLAYING_SEQ_ANIM)
    {
        StopNow();
        Button_PlayRgbSeq->SetLabel("Play (F4)");
        Button_PlayRgbSeq->Enable();
        return;
    }

    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }
    SeqPlayColumn=Grid1->GetGridCursorCol();
    if (SeqPlayColumn < XLIGHTS_SEQ_STATIC_COLUMNS)
    {
        wxMessageBox("Select a cell in a display element column before clicking Play", "Error");
        return;
    }
    NextGridRowToPlay=Grid1->GetGridCursorRow();

    wxString ModelName=Grid1->GetColLabelValue(SeqPlayColumn);
    Choice_Models->SetStringSelection(ModelName);
    int sel=Choice_Models->GetSelection();
    if (sel == wxNOT_FOUND)
    {
        wxMessageBox(_("Can not find model: ")+ModelName, _("Error"));
        return;
    }
    wxXmlNode* ModelXml=(wxXmlNode*)Choice_Models->GetClientData(sel);
    playBuffer.InitBuffer(ModelXml);
    ClearEffectWindow();
    StatusBar1->SetStatusText(_("Playback: RGB sequence"));
    EnableSequenceControls(false);
    PlayCurrentXlightsFile();
    Button_PlayRgbSeq->SetLabel("Stop (F4)");
    Button_PlayRgbSeq->Enable();
    Button_PlayEffect->Disable();
    heartbeat("playback seq", true); //tell fido to start watching -DJ
}

void xLightsFrame::ResetEffectStates(bool *ResetEffectState)
{
    ResetEffectState[0]=true;
    ResetEffectState[1]=true;
}



void xLightsFrame::PlayEffect()
{
    int sel=Choice_Models->GetSelection();
    if (sel == wxNOT_FOUND)
    {
        wxMessageBox(_("No model is selected"), _("ERROR"));
        return;
    }
    if (EffectTreeDlg != NULL)
    {
        EffectTreeDlg->Show(false);
    }
    if (EffectTreeDlg != NULL)
    {
        EffectTreeDlg->Show(false);
    }
    wxXmlNode* ModelXml=(wxXmlNode*)Choice_Models->GetClientData(sel);
    playBuffer.InitBuffer(ModelXml);
    ResetEffectStates(playResetEffectState);
    ClearEffectWindow();
    playBuffer.SetMixType(Choice_LayerMethod->GetStringSelection());
    StatusBar1->SetStatusText(_("Playback: effect"));
    EnableSequenceControls(false);
    ResetTimer(PLAYING_EFFECT);
    Button_PlayEffect->SetLabel(_("Pause Effect (F3)")); //toggle label -DJ
    heartbeat("playback effect", true); //tell fido to start watching -DJ
}

void xLightsFrame::OnButton_PlayEffectClick(wxCommandEvent& event)
{
    if (SeqPlayerState == PLAYING_EFFECT)
    {
        StopNow();
    }
    else
    {
        PlayEffect();
    }
}

void xLightsFrame::EnableSequenceControls(bool enable)
{
    Button_PlayEffect->Enable(/*enable &&*/ Choice_Models->GetCount() > 0); //leave this one enabled -DJ
    Button_PlayRgbSeq->Enable(enable && Grid1->GetNumberCols() > XLIGHTS_SEQ_STATIC_COLUMNS);
    Button_Models->Enable(enable && ModelsNode);
    Button_Presets->Enable(enable && EffectsNode);
    Button_UpdateGrid->Enable(enable);
    Choice_Models->Enable(enable);
    EffectsPanel1->Button_Pictures_Filename->Enable(enable);
    EffectsPanel1->TextCtrl_Pictures_Filename->Enable(enable);
    EffectsPanel1->TextCtrl_Glediator_Filename->Enable(enable);
    EffectsPanel2->Button_Pictures_Filename->Enable(enable);
    EffectsPanel2->TextCtrl_Pictures_Filename->Enable(enable);
    EffectsPanel2->TextCtrl_Glediator_Filename->Enable(enable);
    ButtonSeqExport->Enable(enable && Grid1->GetNumberCols() > XLIGHTS_SEQ_STATIC_COLUMNS);
    ButtonModelExport->Enable(enable && Grid1->GetNumberCols() > XLIGHTS_SEQ_STATIC_COLUMNS);
    BitmapButtonOpenSeq->Enable(enable);
    BitmapButtonSaveSeq->Enable(enable);
    BitmapButtonInsertRow->Enable(enable);
    BitmapButtonDeleteRow->Enable(enable);
    ButtonDisplayElements->Enable(enable && ModelsNode);
    Button_CreateRandom->Enable(enable && Grid1->GetNumberCols() > XLIGHTS_SEQ_STATIC_COLUMNS);
    if (!enable && Grid1->GetNumberCols() > XLIGHTS_SEQ_STATIC_COLUMNS )
    {
        ButtonStopNow->SetFocus();
    }
}

void xLightsFrame::OnButton_PresetsClick(wxCommandEvent& event)
{
    if (EffectTreeDlg==NULL)
    {
        EffectTreeDlg = new EffectTreeDialog(this);
        EffectTreeDlg->InitItems(EffectsNode);
    }
    EffectTreeDlg->Show();
}

void xLightsFrame::SetChoicebook(wxChoicebook* cb, wxString& PageName)
{
    for(size_t i=0; i<cb->GetPageCount(); i++)
    {
        if (cb->GetPageText(i) == PageName)
        {
            cb->SetSelection(i);
            return;
        }
    }
}

//#define WANT_DEBUG_IMPL
//#define WANT_DEBUG  -99 //unbuffered in case app crashes
//#include "djdebug.cpp"
//#ifndef debug_function //dummy defs if debug cpp not included above
// #define debug(level, ...)
// #define debug_more(level, ...)
// #define debug_function(level)
//#endif

#if 0 //obsolete
static wxString prev_model = "junk";
static void load_face_elements(const wxString& model_name, wxCheckListBox* ctrl)
{
#if 0 //hard-coded test
    ctrl->Append(wxT("1: 3# @A1-D12"));
    ctrl->Append(wxT("2: 6# @B1-F12"));
    ctrl->Append(wxT("3: 9# @C1-Z12"));
    ctrl->Append(wxT("4: 2# @E1-Y12"));
    ctrl->Append(wxT("5: 5# @G1-F2"));
#else //use real data from parsed model
    for (auto it = xLightsFrame::PreviewModels.begin(); it != xLightsFrame::PreviewModels.end(); ++it)
    {
        if ((*it)->name == model_name)
            if ((*it)->GetChannelCoords(0, ctrl)) return;
    }
//also check non-preview models:
    for (auto it = xLightsFrame::OtherModels.begin(); it != xLightsFrame::OtherModels.end(); ++it)
    {
        if ((*it)->name == model_name)
            if ((*it)->GetChannelCoords(0, ctrl)) return;
    }
#endif
}
#endif // 0

void xLightsFrame::SetEffectControls(wxString settings, const wxString& model_name)
{
    long TempLong;
    wxColour color;
    wxWindow *CtrlWin, *ContextWin;
    wxString before,after,name,value;
    EffectsPanel *efPanel;
    int cnt=0;

//NOTE: the settings loop after this section does not initialize controls.
//For controls that have been added recently, an older version of the XML file will cause initial settings to be incorrect.
//A loop needs to be added to initialize the wx controls to a predictable value.
//For now, a few recently added controls are explicitly initialized here:
//(not sure if there will be side effects to using a full loop) -DJ
    CheckBox_LayerMorph->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel1->CheckBox_TextToCenter1->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel1->CheckBox_TextToCenter2->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel1->CheckBox_TextToCenter3->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel1->CheckBox_TextToCenter4->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel2->CheckBox_TextToCenter1->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel2->CheckBox_TextToCenter2->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel2->CheckBox_TextToCenter3->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel2->CheckBox_TextToCenter4->SetValue(false); //reset in case not present in settings -DJ
    EffectsPanel1->SingleStrandEffectType->SetSelection(0); //Set to first page in case not present


    while (!settings.IsEmpty())
    {
//NOTE: this doesn't handle "," embedded into Text lines (causes "unable to find" error): -DJ
        before=settings.BeforeFirst(',');
        after=settings.AfterFirst(',');
        switch (cnt)
        {
        case 0:
            SetChoicebook(EffectsPanel1->Choicebook1,before);
            break;
        case 1:
            SetChoicebook(EffectsPanel2->Choicebook1,before);
            break;
        case 2:
            Choice_LayerMethod->SetStringSelection(before);
            break;
        default:
            name=before.BeforeFirst('=');
            if (name.StartsWith("E1_"))
            {
                ContextWin=EffectsPanel1;
                name="ID_"+name.Mid(3);
                efPanel = EffectsPanel1;
            }
            else if (name.StartsWith("E2_"))
            {
                ContextWin=EffectsPanel2;
                name="ID_"+name.Mid(3);
                efPanel = EffectsPanel2;
            }
            else
            {
                efPanel = NULL;
                ContextWin=SeqPanelLeft;
            }
            value=before.AfterFirst('=');
            CtrlWin=wxWindow::FindWindowByName(name,ContextWin);
            if (CtrlWin)
            {
                if (name.StartsWith("ID_SLIDER"))
                {
                    wxSlider* ctrl=(wxSlider*)CtrlWin;
                    if (value.ToLong(&TempLong)) ctrl->SetValue(TempLong);
                }
                else if (name.StartsWith("ID_TEXTCTRL"))
                {
                    value.Replace("&comma;", ",", true); //kludge: remove escape code for "," -DJ
                    wxTextCtrl* ctrl=(wxTextCtrl*)CtrlWin;
                    ctrl->SetValue(value);
                }
                else if (name.StartsWith("ID_CHOICE"))
                {
                    wxChoice* ctrl=(wxChoice*)CtrlWin;
                    ctrl->SetStringSelection(value);
                }
                else if (name.StartsWith("ID_BUTTON"))
                {
                    color.Set(value);
                    if (efPanel != NULL)
                    {
                        efPanel->SetButtonColor((wxButton*)CtrlWin, &color);
                    }
                    else
                    {
                        CtrlWin->SetBackgroundColour(color);
                    }
                    //CtrlWin->SetBackgroundColour(color);
                    //SetTextColor(CtrlWin);
                }
                else if (name.StartsWith("ID_CHECKBOX"))
                {
                    wxCheckBox* ctrl=(wxCheckBox*)CtrlWin;
                    if (value.ToLong(&TempLong)) ctrl->SetValue(TempLong!=0);
                }
                else if (name.StartsWith("ID_NOTEBOOK"))
                {
                    wxNotebook* ctrl=(wxNotebook*)CtrlWin;
                    for (int z = 0 ; z < ctrl->GetPageCount() ; z++)
                    {
                        if (value == ctrl->GetPageText(z))
                        {
                            ctrl->SetSelection(z);
                        }
                    }
                }
#if 0 //obsolete
                else if (name.StartsWith("ID_CHECKLISTBOX")) //for Pgo Coro Face element list
                {
                    wxCheckListBox* ctrl = (wxCheckListBox*)CtrlWin;
//                    ctrl->Clear();
//                    if (!model_name.empty())
                    if (model_name != prev_model) //load face elements from current model
                        load_face_elements(model_name, ctrl);
                    debug(10, "set %s from value '%s', model = '%s'", (const char*)name, (const char*)value.c_str(), (const char*)buffer.name.c_str());
                    wxStringTokenizer wtkz(value, "+");
                    while (wtkz.HasMoreTokens())
                    {
                        wxString nextkey = wtkz.GetNextToken();
                        if (nextkey.empty()) break; //continue;
//                        long keyval; //= wxAtoi(nextinx);
//                        if (nextkey.ToLong(&keyval)) continue;
                        debug(10, "on[%s]", (const char*)nextkey.c_str());
                        if (model_name.empty()) //presets effects tree?
                            if (model_name != prev_model)
                            {
                                ctrl->Append(nextkey); //just use value as-is; other values will be filled in when it's copied into grid
                                ctrl->Check(ctrl->GetCount()); //kludge: turn them all on to preserve them
                                continue;
                            }
                        nextkey = nextkey.BeforeFirst(':'); //strip off the part that can change between models
//                        ctrl->Check(key); //wrong!
                        for (int i = 0; i < ctrl->GetCount(); ++i)
                        {
                            debug(10, "vs. val str[%d/%d] = '%s', key '%s'", i, ctrl->GetCount(), (const char*)ctrl->GetString(i).c_str(), (const char*)ctrl->GetString(i).BeforeFirst(':').c_str());
                            if (ctrl->GetString(i).BeforeFirst(':') == nextkey)
                            {
                                ctrl->Check(i); //match by key (doesn't change), not by index
                                break;
                            }
                        }
                    }
                    prev_model = model_name; //remember which model is cached
                }
#endif // 0
                else
                {
                    wxMessageBox("Unknown type: "+name, "Internal Error");
                }
            }
            else
            {
                wxMessageBox("Unable to find: "+name, "Internal Error");
            }
            break;
        }
        settings=after;
        cnt++;
    }
    // set textbox values for sliders that have them
    wxScrollEvent evt;
    OnSlider_BrightnessCmdScroll(evt);
    OnSlider_ContrastCmdScroll(evt);
    OnSlider_EffectLayerMixCmdScroll(evt);

    OnSlider_SparkleFrequencyCmdScroll(evt);
//    OnSlider_Model_BrightnessCmdScroll(evt);
//   OnSlider_SparkleSliderCmdScroll(evt);

    EffectsPanel1->UpdateSpeedText();
    EffectsPanel2->UpdateSpeedText();

    MixTypeChanged=true;
    FadesChanged=true;
    EffectsPanel1->PaletteChanged=true;
    EffectsPanel2->PaletteChanged=true;
    ResetEffectStates(playResetEffectState);
}

wxXmlNode* xLightsFrame::CreateEffectNode(wxString& name)
{
    wxXmlNode* NewXml=new wxXmlNode(wxXML_ELEMENT_NODE, "effect");
    NewXml->AddAttribute("name", name);
    NewXml->AddAttribute("settings", CreateEffectString());
    return NewXml;
}

//modifed for partially random -DJ
//void djdebug(const char* fmt, ...); //_DJ
wxString xLightsFrame::CreateEffectStringRandom()
{
    int eff1, eff2, layerOp;
    wxString s;
    s.clear();

    /*
     enum RGB_EFFECTS_e
    {
        eff_NONE,
        eff_OFF,
        eff_ON,
        eff_BARS,
        eff_BUTTERFLY,
        eff_CIRCLES,
        eff_COLORWASH,
        eff_COROFACES,
        eff_CURTAIN,
        eff_FACES,
        eff_FIRE,
        eff_FIREWORKS,
        eff_GARLANDS,
        eff_GLEDIATOR,
        eff_LIFE,
        eff_METEORS,
        eff_PIANO,
        eff_PICTURES,
        eff_PINWHEEL,
        eff_RIPPLE,
        eff_SHIMMER,
        eff_SINGLESTRAND,
        eff_SNOWFLAKES,
        eff_SNOWSTORM,
        eff_SPIRALS,
        eff_SPIROGRAPH,
        eff_STROBE,
        eff_TEXT,
        eff_TREE,
        eff_TWINKLE,
        eff_WAVE,
        eff_LASTEFFECT //Always the last entry
    };
    */
    //   Old way
    eff1 = EffectsPanel1->isRandom_()? rand() % eff_LASTEFFECT: EffectsPanel1->Choicebook1->GetSelection();
    eff2 = EffectsPanel2->isRandom_()? rand() % eff_LASTEFFECT: EffectsPanel2->Choicebook1->GetSelection();

    //  new way
    if (EffectsPanel1->isRandom_()) //avoid a few types of random effects
    {
        eff1 = CreateRandomEffect(eff_LASTEFFECT);
        /* eff1 = (eff_NONE == eff1 || eff_TEXT == eff1 || eff_PICTURES == eff1 || eff_PIANO == eff1
                 || eff_FACES == eff1 || eff_COROFACES == eff1 ||eff_GLEDIATOR == eff1
                 || eff_OFF == eff1)? eff_NONE:eff1;
         if(eff1 < eff_NONE || eff1 >= eff_LASTEFFECT) eff1 = eff_NONE;
             */
    }
    else
    {
        eff1 = EffectsPanel1->Choicebook1->GetSelection();
    }
    if (EffectsPanel2->isRandom_()) //avoid a few types of random effects
    {
        eff2 = CreateRandomEffect(eff_LASTEFFECT);
        /*
        eff2 = (eff_NONE == eff2|| eff_TEXT == eff2 || eff_PICTURES == eff2 || eff_PIANO == eff2 // if the above eff2+1 pushes into an effect
                || eff_FACES == eff2 || eff_COROFACES == eff2 ||eff_GLEDIATOR == eff2
                || eff_OFF == eff2)? eff_NONE:eff2;                  // we should skip, just set effect to NONE
        if(eff2 < eff_NONE || eff2 >= eff_LASTEFFECT) eff2 = eff_NONE;
            */
    }
    else
    {
        eff2 = EffectsPanel2->Choicebook1->GetSelection();
    }

    layerOp = isRandom(Slider_EffectLayerMix)? rand() % LASTLAYER: Choice_LayerMethod->GetSelection();
    s = EffectNames[eff1] + ","+EffectNames[eff2] + "," + EffectLayerOptions[layerOp];
#if 0 // <SCM>
    s += ",ID_CHECKBOX_LayerMorph=0";
#else
    s += ",ID_CHECKBOX_LayerMorph=" + wxString::Format("%d", (isRandom(CheckBox_LayerMorph)? rand() & 1: CheckBox_LayerMorph->GetValue())? 1: 0);
#endif // 1
    s += ",ID_SLIDER_SparkleFrequency=" + wxString::Format("%d", isRandom(Slider_SparkleFrequency)? rand() % Slider_SparkleFrequency->GetMax(): Slider_SparkleFrequency->GetValue()); // max is actually all teh way left, ie no sparkles

    //  first calculate it the old way
    int newbrightness = isRandom(Slider_Brightness)? rand() % Slider_Brightness->GetMax(): Slider_Brightness->GetValue();
    newbrightness=100; // but instead overwrite it. no matter what we are creating, lets not mess with brightness
    //  s += ",ID_SLIDER_Brightness=" + wxString::Format("%d", isRandom(Slider_Brightness)? rand() % Slider_Brightness->GetMax(): Slider_Brightness->GetValue());
    s += ",ID_SLIDER_Brightness=" + wxString::Format("%d", newbrightness);

    s += ",ID_SLIDER_Contrast=" + wxString::Format("%d", isRandom(Slider_Contrast)? 0: Slider_Contrast->GetValue()); //use 0 instead of random value?
    s += EffectsPanel1->GetRandomEffectString(eff1);
    s += EffectsPanel2->GetRandomEffectString(eff2);
#if 0 //partially random -DJ
    int PageIdx1=EffectsPanel1->Choicebook1->GetSelection();
    int PageIdx2=EffectsPanel2->Choicebook1->GetSelection();
    // ID_CHOICEBOOK1, ID_CHOICEBOOK2, ID_CHOICE_LayerMethod
    wxString s=EffectsPanel1->Choicebook1->GetPageText(PageIdx1)+","+EffectsPanel2->Choicebook1->GetPageText(PageIdx2);
    s+=","+Choice_LayerMethod->GetStringSelection();
    s+=",ID_SLIDER_SparkleFrequency="+wxString::Format("%d",Slider_SparkleFrequency->GetValue());
    s+=",ID_SLIDER_Brightness="+wxString::Format("%d",Slider_Brightness->GetValue());
    s+=",ID_SLIDER_Contrast="+wxString::Format("%d",Slider_Contrast->GetValue());
    s+=",ID_SLIDER_EffectLayerMix="+wxString::Format("%d",Slider_EffectLayerMix->GetValue());
    s+=EffectsPanel1->GetEffectString();
    s+=EffectsPanel2->GetEffectString();
#elif 0
#define tostr(thing)  #thing
#define EFFECT "Color Wash"
#define SPFREQ  200
#define BRIGHT  109
#define CONTRAST  69
    s= EFFECT ",None,Effect 1,ID_SLIDER_SparkleFrequency= " tostr(SPREQ) ",ID_SLIDER_Brightness=" tostr(BRIGHT) ",ID_SLIDER_Contrast=" tostr(CONTRAST) ",ID_SLIDER_EffectLayerMix=0";
    s+=EffectsPanel1->GetRandomEffectString(eff1);
    s+=EffectsPanel2->GetRandomEffectString(eff2);
#endif
    return s;

}

int xLightsFrame::CreateRandomEffect(int eff_LASTEFFECT)
{
    bool BAD_CHOICE=1;
    int eff,count=0;
    int MAX_TRIES=10;

    //    srand (time(NULL));
    while (BAD_CHOICE && count<MAX_TRIES)
    {
        count++;
        eff=rand() % eff_LASTEFFECT;
        BAD_CHOICE = (eff_NONE == eff || eff_TEXT == eff || eff_PICTURES == eff || eff_PIANO == eff
                      || eff_FACES == eff || eff_COROFACES == eff || eff_GLEDIATOR == eff
                      || eff_OFF == eff || eff_ON == eff);
    }
    if(count==MAX_TRIES) eff=eff_NONE; // we failed to find a good effect after MAX_TRIES attempts
    return eff;
}

wxString xLightsFrame::CreateEffectString()
{
    int PageIdx1=EffectsPanel1->Choicebook1->GetSelection();
    int PageIdx2=EffectsPanel2->Choicebook1->GetSelection();
    // ID_CHOICEBOOK1, ID_CHOICEBOOK2, ID_CHOICE_LayerMethod
    wxString s=EffectsPanel1->Choicebook1->GetPageText(PageIdx1)+","+EffectsPanel2->Choicebook1->GetPageText(PageIdx2);
    s+=","+Choice_LayerMethod->GetStringSelection();
#if 0 // <SCM>
    s += ",ID_CHECKBOX_LayerMorph=0";
#else
    s+=",ID_CHECKBOX_LayerMorph=" + wxString::Format("%d", CheckBox_LayerMorph->GetValue()? 1: 0);
#endif // 1


    s+=",ID_SLIDER_SparkleFrequency="+wxString::Format("%d",Slider_SparkleFrequency->GetValue());
    s+=",ID_SLIDER_Brightness="+wxString::Format("%d",Slider_Brightness->GetValue());
    s+=",ID_SLIDER_Contrast="+wxString::Format("%d",Slider_Contrast->GetValue());
    s+=",ID_SLIDER_EffectLayerMix="+wxString::Format("%d",Slider_EffectLayerMix->GetValue());
    s+=EffectsPanel1->GetEffectString();
    s+=EffectsPanel2->GetEffectString();
    return s;
}
void xLightsFrame::GridCellChanged(int row, int col)
{
    if (row < changedRow)
    {
        changedRow = row;
    }
    if (col < changedColumn)
    {
        changedColumn = col;
    }
    UnsavedChanges = true;
}


void xLightsFrame::UpdateGrid()
{
    int r,c;
    wxString v=CreateEffectString();
    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    if ( Grid1->IsSelection() )
    {
        // iterate over entire grid looking for selected cells
        int nRows = Grid1->GetNumberRows();
        int nCols = Grid1->GetNumberCols();
        for (r=0; r<nRows; r++)
        {
            for (c=XLIGHTS_SEQ_STATIC_COLUMNS; c<nCols; c++)
            {
                if (Grid1->IsInSelection(r,c))
                {
                    Grid1->SetCellValue(r,c,v);
                    GridCellChanged(r, c);
                }
            }
        }
        Grid1->ForceRefresh();
    }
    else
    {
        // copy to current cell
        r=Grid1->GetGridCursorRow();
        c=Grid1->GetGridCursorCol();
        if (c >=XLIGHTS_SEQ_STATIC_COLUMNS)
        {
            Grid1->SetCellValue(r,c,v);
            GridCellChanged(r, c);
        }
    }
}
void xLightsFrame::OnButton_UpdateGridClick(wxCommandEvent& event)
{
    UpdateGrid();
}
void xLightsFrame::InsertRandomEffects(wxCommandEvent& event)
{
    int r,c;
    wxString v;

    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    if ( Grid1->IsSelection() )
    {
        // iterate over entire grid looking for selected cells
        int nRows = Grid1->GetNumberRows();
        int nCols = Grid1->GetNumberCols();
        for (r=0; r<nRows; r++)
        {
            for (c=XLIGHTS_SEQ_STATIC_COLUMNS; c<nCols; c++)
            {
                if (Grid1->IsInSelection(r,c))
                {
                    v = CreateEffectStringRandom();
                    Grid1->SetCellValue(r,c,v);
                    GridCellChanged(r, c);
                }
            }
        }
    }
    else
    {
        // copy to current cell
        r=curCell->GetRow();
        c=curCell->GetCol();
        if (c >=XLIGHTS_SEQ_STATIC_COLUMNS)
        {
            v = CreateEffectStringRandom();
            Grid1->SetCellValue(r,c,v);
            GridCellChanged(r, c);
        }
    }
}

//copy random effect into other cells on this same row -DJ
void xLightsFrame::CopyEffectAcrossRow(wxCommandEvent& event)
{
    int nCols = Grid1->GetNumberCols();
    wxString v;
    int r, c;

    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    if ( Grid1->IsSelection() )
    {
        // iterate over entire grid looking for selected cells
        int nRows = Grid1->GetNumberRows();
        for (r = 0; r < nRows; r++)
        {
            for (c = XLIGHTS_SEQ_STATIC_COLUMNS; c < nCols; c++) //find first selected cell
                if (Grid1->IsInSelection(r,c))
                {
                    v = Grid1->GetCellValue(r, c);
                    break;
                }
            if (c < nCols) //found a selected cell
                for (c = XLIGHTS_SEQ_STATIC_COLUMNS; c < nCols; c++)
                {
                    //copy it to other cells in this row
                    Grid1->SetCellValue(r, c, v);
                    GridCellChanged(r, c);
                }
        }
    }
    else
    {
        r = curCell->GetRow();
        c = curCell->GetCol();
        if (c >= XLIGHTS_SEQ_STATIC_COLUMNS)
        {
            v = Grid1->GetCellValue(r, c); //CreateEffectStringRandom(); //get selected cell text
//wxMessageBox(wxString::Format("col# %d of %d = %s", c, nCols, v));
            for (c = XLIGHTS_SEQ_STATIC_COLUMNS; c < nCols; c++)
            {
                //copy it to other cells in this row
                Grid1->SetCellValue(r, c, v);
                GridCellChanged(r, c);
            }
        }
    }
}

//clear all effects on this row -DJ
void xLightsFrame::ClearEffectRow(wxCommandEvent& event)
{
    int nCols = Grid1->GetNumberCols();
    wxString v;
    int r, c;

    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    if ( Grid1->IsSelection() )
    {
        // iterate over entire grid looking for selected cells
        int nRows = Grid1->GetNumberRows();
        for (r = 0; r < nRows; r++)
        {
            for (c = XLIGHTS_SEQ_STATIC_COLUMNS; c < nCols; c++) //find first selected cell
                if (Grid1->IsInSelection(r,c)) break;
            if (c < nCols)   //found a selected cell
            {
                for (c = XLIGHTS_SEQ_STATIC_COLUMNS; c < nCols; c++)
                {
                    Grid1->SetCellValue(r, c, "");
                    GridCellChanged(r, c);
                }
            }
        }
    }
    else
    {
        r = curCell->GetRow();
        for (c = XLIGHTS_SEQ_STATIC_COLUMNS; c < nCols; c++)
        {
            Grid1->SetCellValue(r, c, "");
            GridCellChanged(r, c);
        }
    }
}

void xLightsFrame::DeleteSelectedEffects(wxCommandEvent& event)
{
    int r,c;
    wxString v;
    v.Clear();

    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    if ( Grid1->IsSelection() )
    {
        // iterate over entire grid looking for selected cells
        int nRows = Grid1->GetNumberRows();
        int nCols = Grid1->GetNumberCols();
        for (r=0; r<nRows; r++)
        {
            for (c=XLIGHTS_SEQ_STATIC_COLUMNS; c<nCols; c++)
            {
                if (Grid1->IsInSelection(r,c))
                {
                    Grid1->SetCellValue(r,c,v);
                    Grid1->SetCellTextColour(r,c,*wxBLACK);
                    GridCellChanged(r, c);
                }
            }
        }
    }
    else
    {
        // copy to current cell
        r=curCell->GetRow();
        c=curCell->GetCol();
        if (c >=XLIGHTS_SEQ_STATIC_COLUMNS)
        {
            Grid1->SetCellValue(r,c,v);
            Grid1->SetCellTextColour(r,c,*wxBLACK);
            GridCellChanged(r, c);
        }
    }
}

void xLightsFrame::ProtectSelectedEffects(wxCommandEvent& event)
{
    int r,c;
    wxString v;
    v.Clear();

    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    if ( Grid1->IsSelection() )
    {
        // iterate over entire grid looking for selected cells
        int nRows = Grid1->GetNumberRows();
        int nCols = Grid1->GetNumberCols();
        for (r=0; r<nRows; r++)
        {
            for (c=XLIGHTS_SEQ_STATIC_COLUMNS; c<nCols; c++)
            {
                if (Grid1->IsInSelection(r,c))
                {
                    Grid1->SetCellTextColour(r,c, *wxBLUE);
                }
            }
        }
    }
    else
    {
        // copy to current cell
        r=curCell->GetRow();
        c=curCell->GetCol();
        if (c >= XLIGHTS_SEQ_STATIC_COLUMNS)
        {
            Grid1->SetCellTextColour(r,c,*wxBLUE);
        }
    }
    Grid1->ForceRefresh();
}


void xLightsFrame::UnprotectSelectedEffects(wxCommandEvent& event)
{
    int r,c;
    wxString v;
    v.Clear();

    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    if ( Grid1->IsSelection() )
    {
        // iterate over entire grid looking for selected cells
        int nRows = Grid1->GetNumberRows();
        int nCols = Grid1->GetNumberCols();
        for (r=0; r<nRows; r++)
        {
            for (c=XLIGHTS_SEQ_STATIC_COLUMNS; c<nCols; c++)
            {
                if (Grid1->IsInSelection(r,c))
                {
                    Grid1->SetCellTextColour(r,c, *wxBLACK);
                }
            }
        }
    }
    else
    {
        // copy to current cell
        r=curCell->GetRow();
        c=curCell->GetCol();
        if (c >= XLIGHTS_SEQ_STATIC_COLUMNS)
        {
            Grid1->SetCellTextColour(r,c,*wxBLACK);
        }
    }
    Grid1->ForceRefresh();
}

void xLightsFrame::OnChoice_LayerMethodSelect(wxCommandEvent& event)
{
    MixTypeChanged=true;
}

void xLightsFrame::ShowModelsDialog()
{
    ModelListDialog dialog(this);
    wxString name;
    wxXmlNode* e;
    for(e=ModelsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "model")
        {
            name=e->GetAttribute("name");
            if (!name.IsEmpty())
            {
                dialog.ListBox1->Append(name,e);
            }
        }
    }
    dialog.HtmlEasyPrint=HtmlEasyPrint;
    dialog.ShowModal();

    // append any new models to the main xml structure
    for(size_t i=0; i<dialog.ListBox1->GetCount(); i++)
    {
        e=(wxXmlNode*)dialog.ListBox1->GetClientData(i);
        if (!e->GetParent())
        {
            ModelsNode->AddChild(e);
        }
    }
    SaveEffectsFile();
    UpdateModelsList();
    EnableSequenceControls(true);
    UpdateChannelNames();
}

void xLightsFrame::OnButton_ModelsClick(wxCommandEvent& event)
{
    ShowModelsDialog();
}

void xLightsFrame::UpdateViewList()
{
    //TODO: Add code to read in model list with v2 values
    wxString name;
    ModelClass *model;
    Choice_Views->Clear();
    Choice_Views->Append("All Models");
    for(wxXmlNode* e=ViewsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "view")
        {
            name=e->GetAttribute("name");
            if (!name.IsEmpty())
            {
                Choice_Views->Append(name,e);
            }
        }
    }
    Choice_Views->SetSelection(0);
    ShowAllModelsView();
}

void xLightsFrame::OnChoice_ViewsSelect(wxCommandEvent& event)
{
    UpdateView();
}

void xLightsFrame::UpdateView()
{
    if(Choice_Views->GetSelection() == 0)
    {
        ShowAllModelsView();
    }
    else
    {
        ShowModelsView();
    }
}

void xLightsFrame::ShowAllModelsView()
{
    int cols = Grid1->GetCols();
    for(int col = XLIGHTS_SEQ_STATIC_COLUMNS; col<cols; col++)
    {
        Grid1->ShowCol(col);
    }
}

void xLightsFrame::ViewHideAllModels()
{
    int cols = Grid1->GetCols();
    for(int col = XLIGHTS_SEQ_STATIC_COLUMNS; col<cols; col++)
    {
        Grid1->HideCol(col);
    }
}

void xLightsFrame::ShowModelsView()
{
    ViewHideAllModels();
    for(wxXmlNode* e=ViewsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "view")
        {
            wxString name=e->GetAttribute("name");
            if(name == Choice_Views->GetString(Choice_Views->GetSelection()))
            {
                wxString views = e->GetAttribute("models");
                wxArrayString viewArr =wxSplit(views,',');
                int cols = Grid1->GetCols();
                for(int col = XLIGHTS_SEQ_STATIC_COLUMNS; col<cols; col++)
                {
                    wxString colModel = Grid1->GetColLabelValue(col);
                    if(viewArr.Index(colModel,false,false)!=wxNOT_FOUND)
                    {
                        Grid1->ShowCol(col);
                    }
                }
                break;
            }
        }
    }
}

void xLightsFrame::UpdateModelsList()
{
    //TODO: Add code to read in model list with v2 values
    wxString name;
    ModelClass *model;
    wxString SelectedStr=Choice_Models->GetStringSelection();
    Choice_Models->Clear();
    ListBoxElementList->Clear();
    PreviewModels.clear();
    OtherModels.clear();
    for(wxXmlNode* e=ModelsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "model")
        {
            name=e->GetAttribute("name");
            if (!name.IsEmpty())
            {
                Choice_Models->Append(name,e);
                if (ModelClass::IsMyDisplay(e))
                {
                    model=new ModelClass;
                    model->SetFromXml(e);
                    ListBoxElementList->Append(name,model);
                    PreviewModels.push_back(ModelClassPtr(model));
                }
                else //keep a list of non-preview models as well -DJ
                {
                    model=new ModelClass;
                    model->SetFromXml(e);
                    OtherModels.push_back(ModelClassPtr(model));
                }
            }
        }
    }

    // select a model if one exists
    if (Choice_Models->GetCount() > 0)
    {
        if (SelectedStr.IsEmpty() || !Choice_Models->SetStringSelection(SelectedStr))
        {
            Choice_Models->SetSelection(0);
        }
        Button_PlayEffect->Enable(play_mode == play_off);
    }
}

void xLightsFrame::ResetEffectsXml()
{
    ModelsNode=NULL;
    EffectsNode=NULL;
    PalettesNode=NULL;
    ViewsNode=NULL;
    ModelGroupsNode=NULL;
    SettingsNode=NULL;
}

wxString xLightsFrame::LoadEffectsFileNoCheck()
{
    ResetEffectsXml();
    wxFileName effectsFile;
    effectsFile.AssignDir( CurrentDir );
    effectsFile.SetFullName(_(XLIGHTS_RGBEFFECTS_FILE));
    if (!effectsFile.FileExists())
    {
        // file does not exist, so create an empty xml doc
        CreateDefaultEffectsXml();
    }
    else if (!EffectsXml.Load( effectsFile.GetFullPath() ))
    {
        wxMessageBox(_("Unable to load RGB effects file"), _("Error"));
        CreateDefaultEffectsXml();
    }
    wxXmlNode* root=EffectsXml.GetRoot();
    if (root->GetName() != "xrgb")
    {
        wxMessageBox(_("Invalid RGB effects file. Press Save File button to start a new file."), _("Error"));
        CreateDefaultEffectsXml();
    }
    for(wxXmlNode* e=root->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "models") ModelsNode=e;
        if (e->GetName() == "effects") EffectsNode=e;
        if (e->GetName() == "palettes") PalettesNode=e;
        if (e->GetName() == "views") ViewsNode=e;
        if (e->GetName() == "modelGroups") ModelGroupsNode=e;
        if (e->GetName() == "settings") SettingsNode=e;
    }
    if (ModelsNode == 0)
    {
        ModelsNode = new wxXmlNode( wxXML_ELEMENT_NODE, "models" );
        root->AddChild( ModelsNode );
    }
    if (EffectsNode == 0)
    {
        EffectsNode = new wxXmlNode( wxXML_ELEMENT_NODE, "effects" );
        EffectsNode->AddAttribute("version", XLIGHTS_RGBEFFECTS_VERSION);
        root->AddChild( EffectsNode );
    }
    if (PalettesNode == 0)
    {
        PalettesNode = new wxXmlNode( wxXML_ELEMENT_NODE, "palettes" );
        root->AddChild( PalettesNode );
    }

    if (ViewsNode == 0)
    {
        ViewsNode = new wxXmlNode( wxXML_ELEMENT_NODE, "views" );
        root->AddChild( ViewsNode );
    }

    if (ModelGroupsNode == 0)
    {
        ModelGroupsNode = new wxXmlNode( wxXML_ELEMENT_NODE, "modelGroups" );
        root->AddChild( ModelGroupsNode );
    }

    if(SettingsNode==0)
    {
        SettingsNode = new wxXmlNode( wxXML_ELEMENT_NODE, "settings" );
        root->AddChild( SettingsNode );
        SetXmlSetting("previewWidth","1280");
        SetXmlSetting("previewHeight","720");
    }
    int previewWidth=wxAtoi(GetXmlSetting("previewWidth","1280"));
    int previewHeight=wxAtoi(GetXmlSetting("previewHeight","720"));
    if (previewWidth==0 || previewHeight==0)
    {
        previewWidth = 1280;
        previewHeight = 720;
    }
    SetPreviewSize(previewWidth,previewHeight);

    mBackgroundImage = GetXmlSetting("backgroundImage","");
    mBackgroundBrightness = wxAtoi(GetXmlSetting("backgroundBrightness","100"));
    Slider_BackgroundBrightness->SetValue(mBackgroundBrightness);
    return effectsFile.GetFullPath();
}

void xLightsFrame::LoadEffectsFile()
{
    wxStopWatch sw; // start a stopwatch timer
    wxString filename=LoadEffectsFileNoCheck();
    // check version, do we need to convert?
    wxString version=EffectsNode->GetAttribute("version", "0000");
    if (version < XLIGHTS_RGBEFFECTS_VERSION)
    {
        // convert file
        FixVersionDifferences( filename );
        // load converted file
        LoadEffectsFileNoCheck();
        // update version
        EffectsNode->DeleteAttribute("version");
        EffectsNode->AddAttribute("version", XLIGHTS_RGBEFFECTS_VERSION);
        // re-save
        EffectsXml.Save( filename );
    }

    UpdateModelsList();
    UpdateViewList();
    float elapsedTime = sw.Time()/1000.0; //msec => sec
    StatusBar1->SetStatusText(wxString::Format(_("'%s' loaded in %4.3f sec."), filename, elapsedTime));
}

// returns true on success
bool xLightsFrame::SaveEffectsFile()
{
    wxFileName effectsFile;
    effectsFile.AssignDir( CurrentDir );
    effectsFile.SetFullName(_(XLIGHTS_RGBEFFECTS_FILE));
    if (!EffectsXml.Save( effectsFile.GetFullPath() ))
    {
        wxMessageBox(_("Unable to save RGB effects file"), _("Error"));
        return false;
    }
    UnsavedChanges=false;
    return true;
}

// PaletteNum should be 1 or 2
void xLightsFrame::UpdateBufferPaletteFromMap(int PaletteNum, MapStringString& SettingsMap, PixelBufferClass &buffer)
{
    wxColourVector newcolors;
    for (int i=1; i<=6; i++)
    {
        if (SettingsMap[wxString::Format("E%d_CHECKBOX_Palette%d",PaletteNum,i)] ==  "1")
        {
            newcolors.push_back(wxColour(SettingsMap[wxString::Format("E%d_BUTTON_Palette%d",PaletteNum,i)]));
        }
    }
    buffer.SetPalette(PaletteNum-1,newcolors);
}

// effectNum is 1 or 2
void xLightsFrame::UpdateBufferFadesFromMap(int effectNum, MapStringString& SettingsMap, PixelBufferClass &buffer)
{
    wxString tmpStr;
    double fadeIn, fadeOut;

    tmpStr = SettingsMap[wxString::Format("E%d_TEXTCTRL_Fadein",effectNum)] ;
    tmpStr.ToDouble(&fadeIn);
    tmpStr = SettingsMap[wxString::Format("E%d_TEXTCTRL_Fadeout",effectNum)] ;
    tmpStr.ToDouble(&fadeOut);

    buffer.SetFadeTimes(effectNum-1, fadeIn, fadeOut);
}

void xLightsFrame::UpdateFitToTimeFromMap(int effectNum, MapStringString& SettingsMap, PixelBufferClass &buffer)
{
    bool fitToTime;

    fitToTime = (SettingsMap[wxString::Format("E%d_CHECKBOX_FitToTime",effectNum)] == "1");

    buffer.SetFitToTime(effectNum-1, fitToTime);
}
void xLightsFrame::UpdateBufferFadesFromCtrl(PixelBufferClass &buffer)
{
    wxString tmpStr;
    double fadeIn, fadeOut;

    tmpStr = EffectsPanel1->TextCtrl_Fadein->GetValue();
    tmpStr.ToDouble(&fadeIn);
    tmpStr = EffectsPanel1->TextCtrl_Fadeout->GetValue();
    tmpStr.ToDouble(&fadeOut);
    buffer.SetFadeTimes(0, fadeIn, fadeOut);

    tmpStr = EffectsPanel2->TextCtrl_Fadein->GetValue();
    tmpStr.ToDouble(&fadeIn);
    tmpStr = EffectsPanel2->TextCtrl_Fadeout->GetValue();
    tmpStr.ToDouble(&fadeOut);
    buffer.SetFadeTimes(1, fadeIn, fadeOut);
}

// layer is 0 or 1
void xLightsFrame::UpdateBufferPalette(EffectsPanel* panel, int layer, PixelBufferClass &buffer)
{
    wxColourVector newcolors;

    if (panel->CheckBox_Palette1->IsChecked()) newcolors.push_back(panel->Button_Palette1->GetBackgroundColour());
    if (panel->CheckBox_Palette2->IsChecked()) newcolors.push_back(panel->Button_Palette2->GetBackgroundColour());
    if (panel->CheckBox_Palette3->IsChecked()) newcolors.push_back(panel->Button_Palette3->GetBackgroundColour());
    if (panel->CheckBox_Palette4->IsChecked()) newcolors.push_back(panel->Button_Palette4->GetBackgroundColour());
    if (panel->CheckBox_Palette5->IsChecked()) newcolors.push_back(panel->Button_Palette5->GetBackgroundColour());
    if (panel->CheckBox_Palette6->IsChecked()) newcolors.push_back(panel->Button_Palette6->GetBackgroundColour());
    buffer.SetPalette(layer,newcolors);
}


class EffectEvent
{
public:
    EffectEvent(int l, int p, MapStringString& sm,
                PixelBufferClass &b, bool *res) : mutex()
    {
        layer = l;
        period = p;
        SettingsMap = &sm;
        buffer = &b;
        ResetEffectState = res;
    }
    wxMutex mutex;
    volatile bool done = false;
    int layer;
    int period;
    MapStringString *SettingsMap;
    PixelBufferClass *buffer;
    bool *ResetEffectState;
    bool returnVal = false;
};
wxMutex thread1Mutex;
wxCondition thread1Condition(thread1Mutex);
std::vector<EffectEvent*>effectsToRender;
std::vector<wxString> renderMessages;


// layer is 0 or 1
bool xLightsFrame::RenderEffectFromMap(int layer, int period, MapStringString& SettingsMap,
                                       PixelBufferClass &buffer, bool *ResetEffectState,
                                       bool bgThread)
{
    bool retval=true;

    wxString LayerStr=layer==0 ? "E1_" : "E2_";
    wxString SpeedStr=SettingsMap[LayerStr+"SLIDER_Speed"];
    buffer.SetLayer(layer,period,wxAtoi(SpeedStr),ResetEffectState[layer]);
    ResetEffectState[layer]=false;
    wxString effect=SettingsMap[LayerStr+"Effect"];
    if (effect == "None")
    {
        retval = false;
    }
    else if (effect == "Off")
    {
        buffer.RenderOff();
    }
    else if (effect == "On")
    {
        buffer.RenderOn(wxAtoi(SettingsMap[LayerStr+"SLIDER_Eff_On_Red"]),
                        wxAtoi(SettingsMap[LayerStr+"SLIDER_Eff_On_Grn"]),
                        wxAtoi(SettingsMap[LayerStr+"SLIDER_Eff_On_Blu"]) );
    }
    else if (effect == "Bars")
    {
        buffer.RenderBars(wxAtoi(SettingsMap[LayerStr+"SLIDER_Bars_BarCount"]),
                          BarEffectDirections.Index(SettingsMap[LayerStr+"CHOICE_Bars_Direction"]),
                          SettingsMap[LayerStr+"CHECKBOX_Bars_Highlight"]=="1",
                          SettingsMap[LayerStr+"CHECKBOX_Bars_3D"]=="1");
    }
    else if (effect == "Butterfly")
    {
        buffer.RenderButterfly(ButterflyEffectColors.Index(SettingsMap[LayerStr+"CHOICE_Butterfly_Colors"]),
                               wxAtoi(SettingsMap[LayerStr+"SLIDER_Butterfly_Style"]),
                               wxAtoi(SettingsMap[LayerStr+"SLIDER_Butterfly_Chunks"]),
                               wxAtoi(SettingsMap[LayerStr+"SLIDER_Butterfly_Skip"]),
                               ButterflyDirection.Index(SettingsMap[LayerStr+"CHOICE_Butterfly_Direction"]));
    }
    else if (effect == "Circles")
    {

        buffer.RenderCircles(wxAtoi(SettingsMap[LayerStr+"SLIDER_Circles_Count"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Circles_Size"]),
                             SettingsMap[LayerStr+"CHECKBOX_Circles_Bounce"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Circles_Collide"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Circles_Random_m"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Circles_Radial"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Circles_Radial_3D"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Circles_Bubbles"]=="1",
                             buffer.BufferWi/2, buffer.BufferHt/2,
                             SettingsMap[LayerStr+"CHECKBOX_Circles_Plasma"]=="1"
                            );

    }
    else if (effect == "Color Wash")
    {
        buffer.RenderColorWash(SettingsMap[LayerStr+"CHECKBOX_ColorWash_HFade"]=="1",
                               SettingsMap[LayerStr+"CHECKBOX_ColorWash_VFade"]=="1",
                               wxAtoi(SettingsMap[LayerStr+"SLIDER_ColorWash_Count"]));
    }
    else if (effect == "Curtain")
    {
        buffer.RenderCurtain(CurtainEdge.Index(SettingsMap[LayerStr+"CHOICE_Curtain_Edge"]),
                             CurtainEffect.Index(SettingsMap[LayerStr+"CHOICE_Curtain_Effect"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Curtain_Swag"]),
                             SettingsMap[LayerStr+"CHECKBOX_Curtain_Repeat"]=="1");
    }
    else if (effect == "Faces")
    {
        buffer.RenderFaces(FacesPhoneme.Index(SettingsMap[LayerStr+"CHOICE_Faces_Phoneme"]));
    }
    else if (effect == "CoroFaces")
    {
        buffer.RenderCoroFaces(SettingsMap[LayerStr+"CHOICE_CoroFaces_Phoneme"],
                               SettingsMap[LayerStr+"CHOICE_CoroFaces_Eyes"],
                               SettingsMap[LayerStr+"CHECKBOX_CoroFaces_Outline"] == "1");
    }
    else if (effect == "Fire")
    {
        buffer.RenderFire(wxAtoi(SettingsMap[LayerStr+"SLIDER_Fire_Height"]),
                          wxAtoi(SettingsMap[LayerStr+"SLIDER_Fire_HueShift"]),
                          SettingsMap[LayerStr+"CHECKBOX_Fire_GrowFire"]=="1");
    }
    else if (effect == "Fireworks")
    {
        buffer.RenderFireworks(wxAtoi(SettingsMap[LayerStr+"SLIDER_Fireworks_Number_Explosions"]),
                               wxAtoi(SettingsMap[LayerStr+"SLIDER_Fireworks_Count"]),
                               wxAtoi(SettingsMap[LayerStr+"SLIDER_Fireworks_Velocity"]),
                               wxAtoi(SettingsMap[LayerStr+"SLIDER_Fireworks_Fade"]));
    }
    else if (effect == "Garlands")
    {
        buffer.RenderGarlands(wxAtoi(SettingsMap[LayerStr+"SLIDER_Garlands_Type"]),
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_Garlands_Spacing"]));
    }
    else if (effect == "Glediator")
    {
        buffer.RenderGlediator(SettingsMap[LayerStr+"TEXTCTRL_Glediator_Filename"]);
    }
    else if (effect == "Life")
    {
        buffer.RenderLife(wxAtoi(SettingsMap[LayerStr+"SLIDER_Life_Count"]),
                          wxAtoi(SettingsMap[LayerStr+"SLIDER_Life_Seed"]));
    }
    else if (effect == "Meteors")
    {
        buffer.RenderMeteors(MeteorsEffectTypes.Index(SettingsMap[LayerStr+"CHOICE_Meteors_Type"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Meteors_Count"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Meteors_Length"]),
                             MeteorsEffect.Index(SettingsMap[LayerStr+"CHOICE_Meteors_Effect"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Meteors_Swirl_Intensity"]));
    }
    else if (effect == "Piano")
    {
        buffer.RenderPiano(PianoEffectStyles.Index(SettingsMap[LayerStr+"CHOICE_Piano_Style"]),
                           wxAtoi(SettingsMap[LayerStr+"SLIDER_Piano_NumKeys"]),
                           wxAtoi(SettingsMap[LayerStr+"SLIDER_Piano_NumRows"]),
                           PianoKeyPlacement.Index(SettingsMap[LayerStr+"CHOICE_Piano_Placement"]),
                           SettingsMap[LayerStr+"CHECKBOX_Piano_Clipping"] == "1",
                           SettingsMap[LayerStr+"TEXTCTRL_Piano_CueFilename"],
                           SettingsMap[LayerStr+"TEXTCTRL_Piano_MapFilename"],
                           SettingsMap[LayerStr+"TEXTCTRL_Piano_ShapeFilename"]);
    }
    else if (effect == "Pictures")
    {
        buffer.RenderPictures(PictureEffectDirections.Index(SettingsMap[LayerStr+"CHOICE_Pictures_Direction"]),
                              SettingsMap[LayerStr+"TEXTCTRL_Pictures_Filename"],
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_Pictures_GifType"]),
                              SettingsMap[LayerStr+"CHECKBOX_MovieIs20FPS"] == "1"
                             );
    }
    else if (effect == "Pinwheel")
    {
        buffer.RenderPinwheel(wxAtoi(SettingsMap[LayerStr+"SLIDER_Pinwheel_Arms"]),
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_Pinwheel_Twist"]),
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_Pinwheel_Thickness"]),
                              SettingsMap[LayerStr+"CHECKBOX_Pinwheel_Rotation"] == "1",
                              wxAtoi(SettingsMap[LayerStr+"CHOICE_Pinwheel_3D"]),
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_PinwheelXC"]),
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_PinwheelYC"]),
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_Pinwheel_ArmSize"]));
    }
    else if (effect == "Ripple")
    {
        buffer.RenderRipple(RippleObjectToDraw.Index(SettingsMap[LayerStr+"CHOICE_Ripple_Object_To_Draw"]),
                            RippleMovement.Index(SettingsMap[LayerStr+"CHOICE_Ripple_Movement"]),
                            wxAtoi(SettingsMap[LayerStr+"SLIDER_Ripple_Thickness"]),
                            SettingsMap[LayerStr+"CHECKBOX_Ripple3D"] == "1" );
    }
    else if (effect == "Shimmer")
    {
        buffer.RenderShimmer(wxAtoi(SettingsMap[LayerStr+"SLIDER_Shimmer_Duty_Factor"]),
                             SettingsMap[LayerStr+"CHECKBOX_Shimmer_Use_All_Colors"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Shimmer_Blink_Timing"]=="1",
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Shimmer_Blinks_Per_Row"]));
    }
    else if (effect == "SingleStrand")
    {
        if ("Skips" == SettingsMap[LayerStr+"NOTEBOOK_SSEFFECT_TYPE"])
        {
            buffer.RenderSingleStrandSkips(
                wxAtoi(SettingsMap[LayerStr+"SLIDER_Skips_BandSize"]),
                wxAtoi(SettingsMap[LayerStr+"SLIDER_Skips_SkipSize"]),
                wxAtoi(SettingsMap[LayerStr+"SLIDER_Skips_StartPos"]),
                SettingsMap[LayerStr+"CHOICE_Skips_Direction"]);
        }
        else
        {
            buffer.RenderSingleStrandChase(
                SingleStrandColors.Index(SettingsMap[LayerStr+"CHOICE_SingleStrand_Colors"]),
                wxAtoi(SettingsMap[LayerStr+"SLIDER_Number_Chases"]),
                wxAtoi(SettingsMap[LayerStr+"SLIDER_Color_Mix1"]),
                wxAtoi(SettingsMap[LayerStr+"SLIDER_Chase_Spacing1"]),
                SingleStrandTypes.Index(SettingsMap[LayerStr+"CHOICE_Chase_Type1"]),
                SettingsMap[LayerStr+"CHECKBOX_Chase_3dFade1"]=="1",
                SettingsMap[LayerStr+"CHECKBOX_Chase_Group_All"]=="1");
        }
    }
    else if (effect == "Snowflakes")
    {
        buffer.RenderSnowflakes(wxAtoi(SettingsMap[LayerStr+"SLIDER_Snowflakes_Count"]),
                                wxAtoi(SettingsMap[LayerStr+"SLIDER_Snowflakes_Type"]));
    }
    else if (effect == "Snowstorm")
    {
        buffer.RenderSnowstorm(wxAtoi(SettingsMap[LayerStr+"SLIDER_Snowstorm_Count"]),
                               wxAtoi(SettingsMap[LayerStr+"SLIDER_Snowstorm_Length"]));
    }
    else if (effect == "Spirals")
    {
        buffer.RenderSpirals(wxAtoi(SettingsMap[LayerStr+"SLIDER_Spirals_Count"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Spirals_Direction"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Spirals_Rotation"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Spirals_Thickness"]),
                             SettingsMap[LayerStr+"CHECKBOX_Spirals_Blend"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Spirals_3D"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Spirals_Grow"]=="1",
                             SettingsMap[LayerStr+"CHECKBOX_Spirals_Shrink"]=="1"
                            );
    }
    else if (effect == "Spirograph")
    {
        buffer.RenderSpirograph(wxAtoi(SettingsMap[LayerStr+"SLIDER_Spirograph_R"]),
                                wxAtoi(SettingsMap[LayerStr+"SLIDER_Spirograph_r"]),
                                wxAtoi(SettingsMap[LayerStr+"SLIDER_Spirograph_d"]),
                                wxAtoi(SettingsMap[LayerStr+"SLIDER_Spirograph_Animate"]));
    }
    else if (effect == "Strobe")
    {
        buffer.RenderStrobe(wxAtoi(SettingsMap[LayerStr+"SLIDER_Number_Strobes"]),
                            wxAtoi(SettingsMap[LayerStr+"SLIDER_Strobe_Duration"]),
                            wxAtoi(SettingsMap[LayerStr+"SLIDER_Strobe_Type"]));
    }
    else if (effect == "Text")
    {
        // this needs to be on the primary thread due to GDI calls
        if (bgThread)
        {
            EffectEvent ev(layer, period, SettingsMap, buffer, ResetEffectState);
            ev.mutex.Lock();
            thread1Mutex.Lock();
            effectsToRender.push_back(&ev);
            thread1Condition.Broadcast();
            thread1Mutex.Unlock();
            ev.mutex.Unlock();
            int cnt = 0;
            while (cnt < 50 && !ev.done)
            {
                wxMilliSleep(1);
                cnt++;
            }
        }
        else
        {

            buffer.RenderText(wxAtoi(SettingsMap[LayerStr+"SLIDER_Text_Position1"]),
                              SettingsMap[LayerStr+"TEXTCTRL_Text_Line1"],
                              SettingsMap[LayerStr+"TEXTCTRL_Text_Font1"],
                              TextEffectDirections.Index(SettingsMap[LayerStr+"CHOICE_Text_Dir1"]),
                              wxAtoi(SettingsMap[LayerStr+"CHECKBOX_TextToCenter1"]) != 0,
                              TextEffects.Index(SettingsMap[LayerStr+"CHOICE_Text_Effect1"]),
                              TextCountDown.Index(SettingsMap[LayerStr+"CHOICE_Text_Count1"]),
                              //
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_Text_Position2"]),
                              SettingsMap[LayerStr+"TEXTCTRL_Text_Line2"],
                              SettingsMap[LayerStr+"TEXTCTRL_Text_Font2"],
                              TextEffectDirections.Index(SettingsMap[LayerStr+"CHOICE_Text_Dir2"]),
                              wxAtoi(SettingsMap[LayerStr+"CHECKBOX_TextToCenter2"]) != 0,
                              TextEffects.Index(SettingsMap[LayerStr+"CHOICE_Text_Effect2"]),
                              TextCountDown.Index(SettingsMap[LayerStr+"CHOICE_Text_Count2"]),
                              //
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_Text_Position3"]),
                              SettingsMap[LayerStr+"TEXTCTRL_Text_Line3"],
                              SettingsMap[LayerStr+"TEXTCTRL_Text_Font3"],
                              TextEffectDirections.Index(SettingsMap[LayerStr+"CHOICE_Text_Dir3"]),
                              wxAtoi(SettingsMap[LayerStr+"CHECKBOX_TextToCenter3"]) != 0,
                              TextEffects.Index(SettingsMap[LayerStr+"CHOICE_Text_Effect3"]),
                              TextCountDown.Index(SettingsMap[LayerStr+"CHOICE_Text_Count3"]),
                              //
                              wxAtoi(SettingsMap[LayerStr+"SLIDER_Text_Position4"]),
                              SettingsMap[LayerStr+"TEXTCTRL_Text_Line4"],
                              SettingsMap[LayerStr+"TEXTCTRL_Text_Font4"],
                              TextEffectDirections.Index(SettingsMap[LayerStr+"CHOICE_Text_Dir4"]),
                              wxAtoi(SettingsMap[LayerStr+"CHECKBOX_TextToCenter4"]) != 0,
                              TextEffects.Index(SettingsMap[LayerStr+"CHOICE_Text_Effect4"]),
                              TextCountDown.Index(SettingsMap[LayerStr+"CHOICE_Text_Count4"]));
        }
    }
    else if (effect == "Tree")
    {
        buffer.RenderTree(wxAtoi(SettingsMap[LayerStr+"SLIDER_Tree_Branches"]));
    }
    else if (effect == "Twinkle")
    {
        buffer.RenderTwinkle(wxAtoi(SettingsMap[LayerStr+"SLIDER_Twinkle_Count"]),
                             wxAtoi(SettingsMap[LayerStr+"SLIDER_Twinkle_Steps"]),
                             SettingsMap[LayerStr+"CHECKBOX_Twinkle_Strobe"]=="1");
    }
    else if (effect == "Wave")
    {

        buffer.RenderWave(WaveType.Index(SettingsMap[LayerStr+"CHOICE_Wave_Type"]), //
                          FillColors.Index(SettingsMap[LayerStr+"CHOICE_Fill_Colors"]),
                          SettingsMap[LayerStr+"CHECKBOX_Mirror_Wave"]=="1",
                          wxAtoi(SettingsMap[LayerStr+"SLIDER_Number_Waves"]),
                          wxAtoi(SettingsMap[LayerStr+"SLIDER_Thickness_Percentage"]),
                          wxAtoi(SettingsMap[LayerStr+"SLIDER_Wave_Height"]),
                          WaveDirection.Index(SettingsMap[LayerStr+"CHOICE_Wave_Direction"]));




    }

    return retval;
}


// layer is 0 or 1
bool xLightsFrame::PlayRgbEffect1(EffectsPanel* panel, int layer, int EffectPeriod)
{
//    wxString parsed;
    bool retval = true;
    bool fitToTime;
    if (panel->EffectChanged)
    {
        playResetEffectState[layer]=true;
        panel->EffectChanged=false;
    }
    if (panel->PaletteChanged)
    {
        UpdateBufferPalette(panel,layer, playBuffer);
        playResetEffectState[layer]=true;
        panel->PaletteChanged=false;
    }
    fitToTime = panel->CheckBox_FitToTime->GetValue();
    playBuffer.SetFitToTime(layer, fitToTime);

//    debug(10, "PlayRgbEffect1(p*, layer %d, eff per %d)", layer, EffectPeriod);
    playBuffer.SetLayer(layer,EffectPeriod,panel->Slider_Speed->GetValue(),playResetEffectState[layer]);
    playResetEffectState[layer]=false;
    switch (panel->Choicebook1->GetSelection())
    {
    case eff_NONE:
        retval = false;
        break;   // none
    case eff_OFF:
        playBuffer.RenderOff();
        break;   // none
    case eff_ON:
        playBuffer.RenderOn(panel->Slider_Eff_On_Red->GetValue(),
                            panel->Slider_Eff_On_Grn->GetValue(),
                            panel->Slider_Eff_On_Blu->GetValue());
        break;   // none
    case eff_BARS:
        playBuffer.RenderBars(panel->Slider_Bars_BarCount->GetValue(),
                              panel->Choice_Bars_Direction->GetSelection(),
                              panel->CheckBox_Bars_Highlight->GetValue(),
                              panel->CheckBox_Bars_3D->GetValue());
        break;
    case eff_BUTTERFLY:
        playBuffer.RenderButterfly(panel->Choice_Butterfly_Colors->GetSelection(),
                                   panel->Slider_Butterfly_Style->GetValue(),
                                   panel->Slider_Butterfly_Chunks->GetValue(),
                                   panel->Slider_Butterfly_Skip->GetValue(),
                                   panel->Choice_Butterfly_Direction->GetSelection());
        break;
    case eff_CIRCLES:
        playBuffer.RenderCircles(panel->Slider_Circles_Count->GetValue(),
                                 panel->Slider_Circles_Size->GetValue(),
                                 panel->CheckBox_Circles_Bounce->GetValue(),
                                 panel->CheckBox_Circles_Collide->GetValue(),
                                 panel->CheckBox_Circles_Random_m->GetValue(),
                                 panel->CheckBox_Circles_Radial->GetValue(),
                                 panel->CheckBox_Circles_Radial_3D->GetValue(),
                                 panel->CheckBox_Circles_Bubbles->GetValue(),
                                 playBuffer.BufferWi/2, playBuffer.BufferHt/2, //temp hard coding.
                                 panel->CheckBox_Circles_Plasma->GetValue()
                                );

        break;
    case eff_COLORWASH:
        playBuffer.RenderColorWash(panel->CheckBox_ColorWash_HFade->GetValue(),
                                   panel->CheckBox_ColorWash_VFade->GetValue(),
                                   panel->Slider_ColorWash_Count->GetValue());
        break;
    case eff_CURTAIN:
        playBuffer.RenderCurtain(panel->Choice_Curtain_Edge->GetSelection(),
                                 panel->Choice_Curtain_Effect->GetSelection(),
                                 panel->Slider_Curtain_Swag->GetValue(),
                                 panel->CheckBox_Curtain_Repeat->GetValue());
        break;
    case eff_FACES:
        playBuffer.RenderFaces(panel->Choice_Faces_Phoneme->GetSelection());
        break;
    case eff_COROFACES:
#if 0
//        wxString parsed;
//kludge: can't change param list (awk script dependency) so pass parsed info in place of non-parsed info
        for (size_t i = 0; i < panel->CheckListBox_CoroFaceElements->GetCount(); ++i)
        {
            if (!panel->CheckListBox_CoroFaceElements->IsChecked(i)) continue;
            if (!parsed.empty()) parsed += wxT("+");
            parsed += panel->CheckListBox_CoroFaceElements->GetString(i);
        }
#endif // 0
        playBuffer.RenderCoroFaces(panel->Choice_CoroFaces_Phoneme->GetString(panel->Choice_CoroFaces_Phoneme->GetSelection()),
                                   panel->Choice_CoroFaces_Eyes->GetString(panel->Choice_CoroFaces_Eyes->GetSelection()),
                                   panel->CheckBox_CoroFaces_Outline->GetValue());
        break;

    case eff_FIRE:
        playBuffer.RenderFire(panel->Slider_Fire_Height->GetValue(),
                              panel->Slider_Fire_HueShift->GetValue(),
                              panel->CheckBox_Fire_GrowFire->GetValue());
        break;
    case eff_FIREWORKS:
        playBuffer.RenderFireworks(panel->Slider_Fireworks_Number_Explosions->GetValue(),
                                   panel->Slider_Fireworks_Count->GetValue(),
                                   panel->Slider_Fireworks_Velocity->GetValue(),
                                   panel->Slider_Fireworks_Fade->GetValue());
        break;
    case eff_GARLANDS:
        playBuffer.RenderGarlands(panel->Slider_Garlands_Type->GetValue(),
                                  panel->Slider_Garlands_Spacing->GetValue());
        break;
    case eff_GLEDIATOR: //changed slider to choice list, added other controls -DJ
        playBuffer.RenderGlediator(panel->TextCtrl_Glediator_Filename->GetValue());
        break;
    case eff_LIFE:
        playBuffer.RenderLife(panel->Slider_Life_Count->GetValue(),
                              panel->Slider_Life_Seed->GetValue());
        break;
    case eff_METEORS:
        playBuffer.RenderMeteors(panel->Choice_Meteors_Type->GetSelection(),
                                 panel->Slider_Meteors_Count->GetValue(),
                                 panel->Slider_Meteors_Length->GetValue(),
                                 panel->Choice_Meteors_Effect->GetSelection(),
                                 panel->Slider_Meteors_Swirl_Intensity->GetValue());
        break;
    case eff_PIANO: //changed slider to choice list, added other controls -DJ
        playBuffer.RenderPiano(panel->Choice_Piano_Style->GetSelection(),
                               panel->Slider_Piano_NumKeys->GetValue(),
                               panel->Slider_Piano_NumRows->GetValue(),
                               panel->Choice_Piano_KeyPlacement->GetSelection(),
                               panel->CheckBox_Piano_Clipping->GetValue(),
                               panel->TextCtrl_Piano_CueFilename->GetValue(),
                               panel->TextCtrl_Piano_MapFilename->GetValue(),
                               panel->TextCtrl_Piano_ShapeFilename->GetValue());
        break;
    case eff_PICTURES:
        playBuffer.RenderPictures(panel->Choice_Pictures_Direction->GetSelection(),
                                  panel->TextCtrl_Pictures_Filename->GetValue(),
                                  panel->Slider_Pictures_GifSpeed->GetValue(),
                                  panel->CheckBox_Pictures_Is20FPS->GetValue());
        break;
    case eff_PINWHEEL:
        playBuffer.RenderPinwheel(panel->Slider_Pinwheel_Arms->GetValue(),
                                  panel->Slider_Pinwheel_Twist->GetValue(),
                                  panel->Slider_Pinwheel_Thickness->GetValue(),
                                  panel->CheckBox_Pinwheel_Rotation->GetValue(),
                                  panel->Choice_Pinwheel_3D->GetSelection(),
                                  panel->Slider_PinwheelXC->GetValue(),
                                  panel->Slider_PinwheelYC->GetValue(),
                                  panel->Slider_Pinwheel_ArmSize->GetValue()
                                 );
        break;
    case eff_RIPPLE:
        playBuffer.RenderRipple(panel->Choice_Ripple_Object_To_Draw->GetSelection(),
                                panel->Choice_Ripple_Movement->GetSelection(),
                                panel->Slider_Ripple_Thickness->GetValue(),
                                panel->CheckBox_Ripple3D->GetValue());
        break;
    case eff_SHIMMER:
        playBuffer.RenderShimmer(panel->Slider_Shimmer_Duty_Factor->GetValue(),
                                 panel->CheckBox_Shimmer_Use_All_Colors->GetValue(),
                                 panel->CheckBox_Shimmer_Blink_Timing->GetValue(),
                                 panel->Slider_Shimmer_Blinks_Per_Row->GetValue());
        break;
    case eff_SINGLESTRAND:
        if ("Skips" == panel->SingleStrandEffectType->GetPageText(panel->SingleStrandEffectType->GetSelection()))
        {
            playBuffer.RenderSingleStrandSkips(panel->Slider_Skips_BandSize->GetValue(),
                                               panel->Slider_Skips_SkipSize->GetValue(),
                                               panel->Slider_Skips_StartPos->GetValue(),
                                               panel->Choice_Skips_Direction->GetString(panel->Choice_Skips_Direction->GetSelection()));
        }
        else
        {
            playBuffer.RenderSingleStrandChase(panel->Choice_SingleStrand_Colors->GetSelection(),
                                               panel->Slider_Number_Chases->GetValue(),
                                               panel->Slider_Color_Mix1->GetValue(),
                                               panel->Slider_Chase_Spacing1->GetValue(),
                                               panel->Choice_Chase_Type1->GetSelection(),
                                               panel->CheckBox_Chase_3dFade1->GetValue(),
                                               panel->CheckBox_Chase_Group_All->GetValue());
        }
        break;
    case eff_SNOWFLAKES:
        playBuffer.RenderSnowflakes(panel->Slider_Snowflakes_Count->GetValue(),
                                    panel->Slider_Snowflakes_Type->GetValue());
        break;
    case eff_SNOWSTORM:
        playBuffer.RenderSnowstorm(panel->Slider_Snowstorm_Count->GetValue(),
                                   panel->Slider_Snowstorm_Length->GetValue());
        break;
    case eff_SPIRALS:
        playBuffer.RenderSpirals(panel->Slider_Spirals_Count->GetValue(),
                                 panel->Slider_Spirals_Direction->GetValue(),
                                 panel->Slider_Spirals_Rotation->GetValue(),
                                 panel->Slider_Spirals_Thickness->GetValue(),
                                 panel->CheckBox_Spirals_Blend->GetValue(),
                                 panel->CheckBox_Spirals_3D->GetValue(),
                                 panel->CheckBox_Spirals_Grow->GetValue(),
                                 panel->CheckBox_Spirlas_Shrink->GetValue());
        break;
    case eff_SPIROGRAPH:
        playBuffer.RenderSpirograph(panel->Slider_Spirograph_R->GetValue(),
                                    panel->Slider_Spirograph_r->GetValue(),
                                    panel->Slider_Spirograph_d->GetValue(),
                                    panel->CheckBox_Spirograph_Animate->GetValue());
        break;
    case eff_STROBE:
        playBuffer.RenderStrobe(panel->Slider_Number_Strobes->GetValue(),
                                panel->Slider_Strobe_Duration->GetValue(),
                                panel->Slider_Strobe_Type->GetValue());
        break;
    case eff_TEXT:
        playBuffer.RenderText(panel->Slider_Text_Position1->GetValue(),
                              panel->TextCtrl_Text_Line1->GetValue(),
                              panel->TextCtrl_Text_Font1->GetValue(),
                              panel->Choice_Text_Dir1->GetSelection(),
                              panel->CheckBox_TextToCenter1->GetValue(),
                              panel->Choice_Text_Effect1->GetSelection(),
                              panel->Choice_Text_Count1->GetSelection(),
                              //
                              panel->Slider_Text_Position2->GetValue(),
                              panel->TextCtrl_Text_Line2->GetValue(),
                              panel->TextCtrl_Text_Font2->GetValue(),
                              panel->Choice_Text_Dir2->GetSelection(),
                              panel->CheckBox_TextToCenter2->GetValue(),
                              panel->Choice_Text_Effect2->GetSelection(),
                              panel->Choice_Text_Count2->GetSelection(),
                              //
                              panel->Slider_Text_Position3->GetValue(),
                              panel->TextCtrl_Text_Line3->GetValue(),
                              panel->TextCtrl_Text_Font3->GetValue(),
                              panel->Choice_Text_Dir3->GetSelection(),
                              panel->CheckBox_TextToCenter3->GetValue(),
                              panel->Choice_Text_Effect3->GetSelection(),
                              panel->Choice_Text_Count3->GetSelection(),
                              //
                              panel->Slider_Text_Position4->GetValue(),
                              panel->TextCtrl_Text_Line4->GetValue(),
                              panel->TextCtrl_Text_Font4->GetValue(),
                              panel->Choice_Text_Dir4->GetSelection(),
                              panel->CheckBox_TextToCenter4->GetValue(),
                              panel->Choice_Text_Effect4->GetSelection(),
                              panel->Choice_Text_Count4->GetSelection());

        break;
    case eff_TREE:
        playBuffer.RenderTree(panel->Slider_Tree_Branches->GetValue());
        break;
    case eff_TWINKLE:
        playBuffer.RenderTwinkle(panel->Slider_Twinkle_Count->GetValue(),
                                 panel->Slider_Twinkle_Steps->GetValue(),
                                 panel->CheckBox_Twinkle_Strobe->GetValue());
        break;
    case eff_WAVE:
        playBuffer.RenderWave(panel->Choice_Wave_Type->GetSelection(),
                              panel->Choice_Fill_Colors->GetSelection(),
                              panel->CheckBox_Mirror_Wave->GetValue(),
                              panel->Slider_Number_Waves->GetValue(),
                              panel->Slider_Thickness_Percentage->GetValue(),
                              panel->Slider_Wave_Height->GetValue(),
                              panel->Choice_Wave_Direction->GetSelection());
        break;

    }
    return retval;
}

//#define WANT_DEBUG 99
//#define WANT_DEBUG_IMPL
//#include "djdebug.cpp"

void xLightsFrame::PlayRgbEffect(int EffectPeriod)
{
    wxString s;
//    debug(1, "play rgb: ovl %d, %d", EffectsPanel1->WantOverlayBkg(), EffectsPanel2->WantOverlayBkg());
//    if (!EffectsPanel1->WantOverlayBkg() && !EffectsPanel2->WantOverlayBkg()) //allow effects to overlay onto other effects (useful for composite models) -DJ
//        buffer.Clear();
    if ((EffectsPanel1->Choicebook1->GetSelection() == eff_NONE) || !EffectsPanel1->WantOverlayBkg())
        playBuffer.Clear(0); //allow effects to overlay onto other effects (useful for composite models) -DJ
    if ((EffectsPanel2->Choicebook1->GetSelection() == eff_NONE) || !EffectsPanel2->WantOverlayBkg())
        playBuffer.Clear(1); //allow effects to overlay onto other effects (useful for composite models) -DJ

    // update SparkleFrequency
    int freq=Slider_SparkleFrequency->GetValue();
    if (freq == Slider_SparkleFrequency->GetMax()) freq=0;
    playBuffer.SetSparkle(freq);

    int brightness=Slider_Brightness->GetValue();
    playBuffer.SetBrightness(brightness);

    int contrast=Slider_Contrast->GetValue();
    playBuffer.SetContrast(contrast);

    int effectMixThreshold=Slider_EffectLayerMix->GetValue();
    playBuffer.SetMixThreshold(effectMixThreshold, CheckBox_LayerMorph->GetValue()); //allow threshold to vary -DJ
//    debug(1, "play rgb effect[%d]: set mix thresh %d, varies? %d", EffectPeriod, effectMixThreshold, CheckBox_LayerMorph->GetValue());

    if (MixTypeChanged)
    {
        s=Choice_LayerMethod->GetStringSelection();
        playBuffer.SetMixType(s);
        MixTypeChanged=false;
    }
    if(FadesChanged)
    {
        UpdateBufferFadesFromCtrl(playBuffer);
        FadesChanged=false;
    }

    heartbeat("playback active", false); //tell fido we're still alive -DJ
//    ResetEffectDuration();
    PlayRgbEffect1(EffectsPanel1, 0, EffectPeriod);
    PlayRgbEffect1(EffectsPanel2, 1, EffectPeriod);
    playBuffer.CalcOutput(EffectPeriod);
    playBuffer.DisplayEffectOnWindow(seqPreview,mPointSize);
    size_t chnum;
    wxByte intensity;
    if (CheckBoxLightOutput->IsChecked() && xout)
    {
        size_t NodeCnt=playBuffer.GetNodeCount();
        size_t cn=playBuffer.ChannelsPerNode();
        for(size_t n=0; n<NodeCnt; n++)
        {
            for(size_t c=0; c<cn; c++)
            {
                playBuffer.GetChanIntensity(n,c,&chnum,&intensity);
                xout->SetIntensity(chnum,intensity);
            }
        }
    }
}



void xLightsFrame::UpdateRgbPlaybackStatus(int seconds, long msec, int EffectPeriod, const wxString& seqtype)
{

    int s=seconds%60;
//    sparkle_count=0;
    msec = (EffectPeriod*50)%1000; // change frame into ms and then find the fractional part
    int minutes=seconds / 60;

    //  TextCtrlPreviewTime->SetValue(wxString::Format("%d:%02d.%03d",minutes,seconds,msec));
    StatusBar1->SetStatusText(wxString::Format("Playback: RGB "+seqtype+" sequence %d:%02d.%03ld   %d.%03ld ",minutes,s,msec,seconds,msec));
//  old way     StatusBar1->SetStatusText(wxString::Format("Playback: RGB "+seqtype+" sequence %d:%02d",m,s));
}

void xLightsFrame::TimerRgbSeq(long msec)
{
    long StartTime;
    int EffectPeriod;
    static int s_period=0;
    int rowcnt=Grid1->GetNumberRows();
    wxString EffectStr;
    switch (SeqPlayerState)
    {
    case PLAYING_EFFECT:
        PlayRgbEffect(s_period);
        playBuffer.SetFadeTimes(0,0.0,0.0);
        playBuffer.SetFadeTimes(1,0.0,0.0);
        s_period++;
        break;
    case STARTING_SEQ_ANIM:
        ResetTimer(PLAYING_SEQ_ANIM, GetGridStartTimeMSec(NextGridRowToPlay));
        playBuffer.SetFadeTimes(0,0.0,0.0);
        playBuffer.SetFadeTimes(1,0.0,0.0);
        xLightsFrame::PlaybackMarker = "0,0"; //keep track of where we are within grid -DJ
        break;
    case PLAYING_SEQ_ANIM:
        if (xout && !xout->TxEmpty())
        {
            TxOverflowCnt++;
            TxOverflowTotal += xout->TxNonEmptyCount(); //show how much -DJ
//            break; //keep going; might catch up -DJ
        }
        EffectPeriod = msec / XTIMER_INTERVAL;
        if (EffectPeriod >= SeqNumPeriods)
        {
            // sequence has finished
            if (xout) xout->alloff();
            ResetTimer(NO_SEQ);
            EnableSequenceControls(true);
            Button_PlayEffect->SetLabel("Play Effect (F3)");
            Button_PlayRgbSeq->SetLabel("Play (F4)");
        }
        else
        {
            if (NextGridRowToPlay < rowcnt && msec >= GetGridStartTimeMSec(NextGridRowToPlay))
            {
                // start next effect
                Grid1->GoToCell(NextGridRowToPlay,SeqPlayColumn);
                Grid1->SelectBlock(NextGridRowToPlay,SeqPlayColumn,NextGridRowToPlay,SeqPlayColumn);

                EffectStr=Grid1->GetCellValue(NextGridRowToPlay,SeqPlayColumn);
                EffectStr.Trim();
                if(!EffectStr.IsEmpty())
                {
                    SetEffectControls(EffectStr, Grid1->GetColLabelValue(SeqPlayColumn));
                    xLightsFrame::PlaybackMarker = wxString::Format("%d,%d", SeqPlayColumn, NextGridRowToPlay); //keep track of where we are within grid -DJ
                }
                playBuffer.SetFitToTime(0, (EffectsPanel1->CheckBox_FitToTime->IsChecked()));
                playBuffer.SetFitToTime(1, (EffectsPanel2->CheckBox_FitToTime->IsChecked()));
                UpdateEffectDuration(!EffectStr.IsEmpty(), NextGridRowToPlay, playBuffer, SeqPlayColumn);
                NextGridRowToPlay++;


            }
            PlayRgbEffect(EffectPeriod);
            UpdateRgbPlaybackStatus(EffectPeriod/20,msec,EffectPeriod,"animation");
//            if (EffectPeriod % 20 == 0) UpdateRgbPlaybackStatus(EffectPeriod/20,msec,"animation");
        }
        break;
    case STARTING_SEQ:
        StartTime=GetGridStartTimeMSec(NextGridRowToPlay);
        if(PlayerDlg->MediaCtrl->GetState() == wxMEDIASTATE_PLAYING)
        {
            ResetTimer(PLAYING_SEQ, StartTime);
            playBuffer.SetFadeTimes(0,0.0,0.0);
            playBuffer.SetFadeTimes(1,0.0,0.0);
        }
        else
        {
            PlayerDlg->MediaCtrl->Seek(StartTime);
            PlayerDlg->MediaCtrl->Play();
        }
        break;
    case PLAYING_SEQ:
        if (xout && !xout->TxEmpty())
        {
            TxOverflowCnt++;
            TxOverflowTotal += xout->TxNonEmptyCount(); //show how much -DJ
//            break; //keep going; might catch up -DJ
        }
        msec = PlayerDlg->MediaCtrl->Tell();
        EffectPeriod = msec / XTIMER_INTERVAL;
        if (EffectPeriod >= SeqNumPeriods || PlayerDlg->MediaCtrl->GetState() != wxMEDIASTATE_PLAYING)
        {
            // sequence has finished
            PlayerDlg->MediaCtrl->Stop();
            if (xout) xout->alloff();
            ResetTimer(NO_SEQ);
            EnableSequenceControls(true);
            Button_PlayEffect->SetLabel("Play Effect (F3)");
            Button_PlayRgbSeq->SetLabel("Play (F4)");
        }
        else
        {
            if (NextGridRowToPlay < rowcnt && msec >= GetGridStartTimeMSec(NextGridRowToPlay))
            {
                // start next effect
                Grid1->MakeCellVisible(NextGridRowToPlay,SeqPlayColumn);
                Grid1->SelectBlock(NextGridRowToPlay,SeqPlayColumn,NextGridRowToPlay,SeqPlayColumn);
                EffectStr=Grid1->GetCellValue(NextGridRowToPlay,SeqPlayColumn);
                EffectStr.Trim();
                if(!EffectStr.IsEmpty())
                {
                    SetEffectControls(EffectStr, Grid1->GetColLabelValue(SeqPlayColumn));
                }
                playBuffer.SetFitToTime(0, (EffectsPanel1->CheckBox_FitToTime->IsChecked()));
                playBuffer.SetFitToTime(1, (EffectsPanel2->CheckBox_FitToTime->IsChecked()));
                UpdateEffectDuration(!EffectStr.IsEmpty(), NextGridRowToPlay, playBuffer, SeqPlayColumn);
                NextGridRowToPlay++;

            }
            PlayRgbEffect(EffectPeriod);
            //TextCtrlLog->AppendText(wxString::Format("msec=%ld, period=%d\n",msec,EffectPeriod));
            UpdateRgbPlaybackStatus(EffectPeriod/20,msec,EffectPeriod,"music");
            //   if (EffectPeriod % 20 == 0) UpdateRgbPlaybackStatus(EffectPeriod/20,msec,"music");
        }
        break;
    }
}

void xLightsFrame::ResetEffectDuration(PixelBufferClass &buffer)
{
    buffer.SetTimes(0, 0, 0, 0, true);
    buffer.SetTimes(1, 0, 0, 0, true);
}
int xLightsFrame::UpdateEffectDuration(bool new_effect_starts, int startRow,
                                       PixelBufferClass &buffer, int playCol)
{
    int ii, curEffMsec, nextEffMsec, nextTimePeriodMsec;
    double val;
    int rowcnt=Grid1->GetNumberRows();
    wxString tmpStr;

    tmpStr = Grid1->GetCellValue(startRow,0);
    curEffMsec =tmpStr.ToDouble(&val )?(int)(val*1000):0;
    ii = 1;
    if (startRow+ii < rowcnt)
    {
        tmpStr = Grid1->GetCellValue(startRow+ii,0);
        nextTimePeriodMsec =tmpStr.ToDouble(&val )?(int)(val*1000):SeqNumPeriods*XTIMER_INTERVAL;
        do
        {
            tmpStr = Grid1->GetCellValue(startRow+ii, playCol);
        }
        while (tmpStr.IsEmpty() && ++ii && startRow+ii < rowcnt);
        //Really taking advantage of short circuit evluation here
        if (!tmpStr.IsEmpty())
        {
            tmpStr = Grid1->GetCellValue(startRow+ii,0);
            nextEffMsec = tmpStr.ToDouble(&val )?(int)(val*1000):SeqNumPeriods*XTIMER_INTERVAL;
        }
        else
        {
            nextEffMsec = SeqNumPeriods*XTIMER_INTERVAL;
        }
    }
    else
    {
        nextEffMsec = nextTimePeriodMsec = SeqNumPeriods*XTIMER_INTERVAL;
    }
    buffer.SetTimes(0, curEffMsec, nextEffMsec, nextTimePeriodMsec, new_effect_starts);
    buffer.SetTimes(1, curEffMsec, nextEffMsec, nextTimePeriodMsec, new_effect_starts);
    return startRow + ii;
}

void xLightsFrame::OnButton_PaletteClick(wxCommandEvent& event)
{
    PaletteMgmtDialog dialog(this);
    dialog.initialize(PalettesNode,EffectsPanel1,EffectsPanel2);
    dialog.ShowModal();
    SaveEffectsFile();
}

void xLightsFrame::DisplayXlightsFilename(const wxString& filename)
{
    xlightsFilename=filename;
    StaticTextSequenceFileName->SetLabel(filename);
    StaticTextPreviewFileName->SetLabel(filename);
    bool EnableButtons=!filename.IsEmpty();
    bbPlayPause->Enable(EnableButtons);
    bbStop->Enable(EnableButtons);
}

void xLightsFrame::GetSeqModelNames(wxArrayString& a)
{
    wxString name;
    for(int i=XLIGHTS_SEQ_STATIC_COLUMNS; i < Grid1->GetCols(); i++)
    {
        name=Grid1->GetColLabelValue(i);
        a.Add(name);
    }
}

void xLightsFrame::GetModelNames(wxArrayString& a)
{
    wxString name;
    for(wxXmlNode* e=ModelsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "model")
        {
            name=e->GetAttribute("name");
            if (!name.IsEmpty())
            {
                a.Add(name);
            }
        }
    }
}

void xLightsFrame::GetGridColumnLabels(wxArrayString& a)
{
    int n=Grid1->GetNumberCols();
    for(int i=XLIGHTS_SEQ_STATIC_COLUMNS; i < n; i++)
    {
        a.Add(Grid1->GetColLabelValue(i));
    }
}

void xLightsFrame::ChooseModelsForSequence()
{
    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }
    SeqParmsDialog dialog(this);
    wxXmlNode* e;
    wxString name;
    int idx;

    // get list of models that are already in the grid
    wxArrayString labels;
    GetGridColumnLabels(labels);

    // populate the listbox with all models
    for(e=ModelsNode->GetChildren(); e!=NULL; e=e->GetNext() )
    {
        if (e->GetName() == "model")
        {
            name=e->GetAttribute("name");
            // allow only models where MyDisplay is set?
            //if (!name.IsEmpty() && e->GetAttribute("MyDisplay","0")=="1") {
            if (!name.IsEmpty())
            {
                dialog.CheckListBox1->Append(name);
                idx=dialog.CheckListBox1->FindString(name);
                if (idx != wxNOT_FOUND && labels.Index(name,false) != wxNOT_FOUND)
                {
                    dialog.CheckListBox1->Check(idx,true);
                }
            }
        }
    }


    dialog.StaticText_Filename->SetLabel(xlightsFilename);
    if (dialog.ShowModal() != wxID_OK) return;

    // add checked models to grid

    size_t cnt = dialog.CheckListBox1->GetCount();
    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    for (size_t i=0; i < cnt; i++)
    {
        if (dialog.CheckListBox1->IsChecked(i))
        {
            name=dialog.CheckListBox1->GetString(i);
            idx=labels.Index(name,false);
            if (idx == wxNOT_FOUND)
            {
                // newly checked item, so add it
                Grid1->AppendCols();
                int colnum=Grid1->GetNumberCols()-1;
                Grid1->SetColLabelValue(colnum,name);
                GridCellChanged(0, colnum);

                for(int j = 0; j < Grid1->GetNumberRows(); j++)
                {
                    Grid1->SetCellOverflow(j,colnum, false);
                    Grid1->SetReadOnly(j,colnum,false);
                }
            }
            else
            {
                // item already exists
                labels[idx].Clear();
            }
        }
    }

    // any non-empty entries in labels represent items that were unchecked - so delete them from grid
    for(idx=labels.GetCount()-1; idx >= 0; idx--)
    {
        if (!labels[idx].IsEmpty())
        {
            Grid1->DeleteCols(idx+XLIGHTS_SEQ_STATIC_COLUMNS);
            GridCellChanged(0, idx+XLIGHTS_SEQ_STATIC_COLUMNS - 1);
        }
    }
    EnableSequenceControls(true);
}


void xLightsFrame::OnButton_ChannelMapClick(wxCommandEvent& event)
{
    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }
    ChannelMapDialog dialog(this);
    dialog.SpinCtrlBaseChannel->SetValue(SeqBaseChannel);
    dialog.CheckBox_EnableBasic->SetValue(SeqChanCtrlBasic);
    dialog.CheckBox_EnableColor->SetValue(SeqChanCtrlColor);
    dialog.SetNetInfo(&NetInfo);
    if (dialog.ShowModal() != wxID_OK) return;
    SeqBaseChannel=dialog.SpinCtrlBaseChannel->GetValue();
    SeqChanCtrlBasic=dialog.CheckBox_EnableBasic->GetValue();
    SeqChanCtrlColor=dialog.CheckBox_EnableColor->GetValue();
}

#include <wx/textfile.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

wxString xLightsFrame::InsertMissing(wxString str,wxString missing_array,bool INSERT)
{
    int pos;
    wxStringTokenizer tkz(missing_array, "|");
    wxString replacement;
    wxString token1 = tkz.GetNextToken(); // get first two dummy tokens out
    wxString token2 = tkz.GetNextToken();
    while ( tkz.HasMoreTokens() )
    {
        token1 = tkz.GetNextToken();
        token2 = tkz.GetNextToken();
        pos=str.find(token1,0);
        replacement = "," + token2 + "</td>";
        if(pos<=0 && INSERT) // if we are INSERT mode we will add token 2 to the end of the xml string
        {
            str.Replace("</td>",replacement);
        }
        else if(pos>0 && !INSERT) // if we are in REPLACE mode (!INSERT), we replace token1 with token 2
        {
            str.Replace(token1,token2);
        }
    }
    return str;
}

// file should be full path
void xLightsFrame::FixVersionDifferences(wxString file)
{
    wxString        str,fileout;
    wxTextFile      tfile;
    wxFile f;
    bool modified=false;
    fileout = file + ".out";
    //wxMessageBox(_("fixing: ")+file);
// open the file
    wxString missing     = "xdummy|xdummy";
    wxString replace_str = "xdummy|xdummy";
    wxString Text1       = "xdummy|xdummy";
    wxString Text2       = "xdummy|xdummy";
    wxString Meteors1    = "xdummy|xdummy";
    wxString Meteors2    = "xdummy|xdummy";
    wxString Fire1       = "xdummy|xdummy";
    wxString Fire2       = "xdummy|xdummy";
    //
    //
    //  list all new tags that might have occured in previous versions
    //  list is pair. first token is what to search for, if it is missing, then put in 2nd token into xml string
    //
    missing      = missing + "|ID_SLIDER_Brightness|ID_SLIDER_Brightness=100";
    missing      = missing + "|ID_SLIDER_Contrast|ID_SLIDER_Contrast=0";

    /*  missing      = missing + "|ID_SLIDER_EffectLayerMix|ID_SLIDER_EffectLayerMix=0";
      missing      = missing + "|ID_TEXTCTRL_Effect1_Fadein|ID_TEXTCTRL_Effect1_Fadein=0";
      missing      = missing + "|ID_TEXTCTRL_Effect1_Fadeout|ID_TEXTCTRL_Effect1_Fadeout=0";
      missing      = missing + "|ID_TEXTCTRL_Effect2_Fadein|ID_TEXTCTRL_Effect2_Fadein=0";
      missing      = missing + "|ID_TEXTCTRL_Effect2_Fadeout|ID_TEXTCTRL_Effect2_Fadeout=0";
      missing      = missing + "|ID_CHECKBOX_Effect1_Fit|ID_CHECKBOX_Effect1_Fit=0";
      missing      = missing + "|ID_CHECKBOX_Effect2_Fit|ID_CHECKBOX_Effect2_Fit=0";
      */

    /*
    ID_SLIDER_EffectLayerMix=0,E1_TEXTCTRL_Fadein=0,E1_TEXTCTRL_Fadeout=0,E2_TEXTCTRL_Fadein=0
    ,E2_TEXTCTRL_Fadeout=0,E1_CHECKBOX_FitToTime=0,E2_CHECKBOX_FitToTime=0,ID_TEXTCTRL_Effect1_Fadein=0
    ,ID_TEXTCTRL_Effect1_Fadeout=0,ID_TEXTCTRL_Effect2_Fadein=0,ID_TEXTCTRL_Effect2_Fadeout=0
    ,ID_CHECKBOX_Effect1_Fit=0,ID_CHECKBOX_Effect2_Fit=0
    */
    //

//   Meteors1 = Meteors1 + "|ID_CHECKBOX_Meteors1_FallUp|ID_CHECKBOX_Meteors1_FallUp=0";
//    Meteors2 = Meteors2 + "|ID_CHECKBOX_Meteors2_FallUp|ID_CHECKBOX_Meteors2_FallUp=0";
    Meteors1 = Meteors1 + "|ID_CHOICE_Meteors1_Effect|ID_CHOICE_Meteors1_Effect=Meteor";
    Meteors2 = Meteors2 + "|ID_CHOICE_Meteors2_Effect|ID_CHOICE_Meteors2_Effect=Meteor";
    Meteors1 = Meteors1 + "|ID_SLIDER_Meteors1_Swirl_Intensity|ID_SLIDER_Meteors1_Swirl_Intensity=10";
    Meteors2 = Meteors2 + "|ID_SLIDER_Meteors2_Swirl_Intensity|ID_SLIDER_Meteors2_Swirl_Intensity=10";

    Fire1 = Fire1 + "|ID_SLIDER_Fire1_HueShift|ID_SLIDER_Fire1_HueShift=0";
    Fire2 = Fire2 + "|ID_SLIDER_Fire2_HueShift|ID_SLIDER_Fire2_HueShift=0";
    Fire1 = Fire1 + "|ID_CHECKBOX_Fire1_GrowFire|ID_CHECKBOX_Fire1_GrowFire=0";
    Fire2 = Fire2 + "|ID_CHECKBOX_Fire2_GrowFire|ID_CHECKBOX_Fire2_GrowFire=0";


    // Lots of variables to check for  text effect
//    ,E1_TEXTCTRL_Text_Line1=God Bless the USA
//	,E1_TEXTCTRL_Text_Line2=God Bless The USA
//	,E1_TEXTCTRL_Text_Font1=
//	,E1_CHOICE_Text_Dir1=left
//	,E1_CHOICE_Text_Effect1=normal
//	,E1_CHOICE_Text_Count1=none
//	,E1_SLIDER_Text_Position1=45



    replace_str = replace_str + "|ID_BUTTON_Palette1_1|E1_BUTTON_Palette1";
    replace_str = replace_str + "|ID_BUTTON_Palette1_2|E1_BUTTON_Palette2";
    replace_str = replace_str + "|ID_BUTTON_Palette1_3|E1_BUTTON_Palette3";
    replace_str = replace_str + "|ID_BUTTON_Palette1_4|E1_BUTTON_Palette4";
    replace_str = replace_str + "|ID_BUTTON_Palette1_5|E1_BUTTON_Palette5";
    replace_str = replace_str + "|ID_BUTTON_Palette1_6|E1_BUTTON_Palette6";
    replace_str = replace_str + "|ID_BUTTON_Palette2_1|E2_BUTTON_Palette1";
    replace_str = replace_str + "|ID_BUTTON_Palette2_2|E2_BUTTON_Palette2";
    replace_str = replace_str + "|ID_BUTTON_Palette2_3|E2_BUTTON_Palette3";
    replace_str = replace_str + "|ID_BUTTON_Palette2_4|E2_BUTTON_Palette4";
    replace_str = replace_str + "|ID_BUTTON_Palette2_5|E2_BUTTON_Palette5";
    replace_str = replace_str + "|ID_BUTTON_Palette2_6|E2_BUTTON_Palette6";
    replace_str = replace_str + "|ID_CHECKBOX_Bars1_3D|E1_CHECKBOX_Bars_3D";
    replace_str = replace_str + "|ID_CHECKBOX_Bars1_Highlight|E1_CHECKBOX_Bars_Highlight";
    replace_str = replace_str + "|ID_CHECKBOX_Bars2_3D|E2_CHECKBOX_Bars_3D";
    replace_str = replace_str + "|ID_CHECKBOX_Bars2_Highlight|E2_CHECKBOX_Bars_Highlight";
    replace_str = replace_str + "|ID_CHECKBOX_Circles1_Bounce|E1_CHECKBOX_Circles_Bounce";
    replace_str = replace_str + "|ID_CHECKBOX_Circles1_Collide|E1_CHECKBOX_Circles_Collide";
    replace_str = replace_str + "|ID_CHECKBOX_Circles1_Radial|E1_CHECKBOX_Circles_Radial";
    replace_str = replace_str + "|ID_CHECKBOX_Circles1_Random_m|E1_CHECKBOX_Circles_Random_m";
    replace_str = replace_str + "|ID_CHECKBOX_Circles2_Bounce|E2_CHECKBOX_Circles_Bounce";
    replace_str = replace_str + "|ID_CHECKBOX_Circles2_Collide|E2_CHECKBOX_Circles_Collide";
    replace_str = replace_str + "|ID_CHECKBOX_Circles2_Radial|E2_CHECKBOX_Circles_Radial";
    replace_str = replace_str + "|ID_CHECKBOX_Circles2_Random_m|E2_CHECKBOX_Circles_Random_m";
    replace_str = replace_str + "|ID_CHECKBOX_ColorWash1_HFade|E1_CHECKBOX_ColorWash_HFade";
    replace_str = replace_str + "|ID_CHECKBOX_ColorWash1_VFade|E1_CHECKBOX_ColorWash_VFade";
    replace_str = replace_str + "|ID_CHECKBOX_ColorWash2_HFade|E2_CHECKBOX_ColorWash_HFade";
    replace_str = replace_str + "|ID_CHECKBOX_ColorWash2_VFade|E2_CHECKBOX_ColorWash_VFade";
    replace_str = replace_str + "|ID_CHECKBOX_Fire1_GrowFire|E1_CHECKBOX_Fire_GrowFire";
    replace_str = replace_str + "|ID_CHECKBOX_Fire2_GrowFire|E2_CHECKBOX_Fire_GrowFire";
//    replace_str = replace_str + "|ID_CHECKBOX_Meteors1_FallUp|E1_CHECKBOX_Meteors_FallUp";
//    replace_str = replace_str + "|ID_CHECKBOX_Meteors2_FallUp|E2_CHECKBOX_Meteors_FallUp";
    replace_str = replace_str + "|ID_CHECKBOX_Palette1_1|E1_CHECKBOX_Palette1";
    replace_str = replace_str + "|ID_CHECKBOX_Palette1_2|E1_CHECKBOX_Palette2";
    replace_str = replace_str + "|ID_CHECKBOX_Palette1_3|E1_CHECKBOX_Palette3";
    replace_str = replace_str + "|ID_CHECKBOX_Palette1_4|E1_CHECKBOX_Palette4";
    replace_str = replace_str + "|ID_CHECKBOX_Palette1_5|E1_CHECKBOX_Palette5";
    replace_str = replace_str + "|ID_CHECKBOX_Palette1_6|E1_CHECKBOX_Palette6";
    replace_str = replace_str + "|ID_CHECKBOX_Palette2_1|E2_CHECKBOX_Palette1";
    replace_str = replace_str + "|ID_CHECKBOX_Palette2_2|E2_CHECKBOX_Palette2";
    replace_str = replace_str + "|ID_CHECKBOX_Palette2_3|E2_CHECKBOX_Palette3";
    replace_str = replace_str + "|ID_CHECKBOX_Palette2_4|E2_CHECKBOX_Palette4";
    replace_str = replace_str + "|ID_CHECKBOX_Palette2_5|E2_CHECKBOX_Palette5";
    replace_str = replace_str + "|ID_CHECKBOX_Palette2_6|E2_CHECKBOX_Palette6";
    replace_str = replace_str + "|ID_CHECKBOX_Spirals1_3D|E1_CHECKBOX_Spirals_3D";
    replace_str = replace_str + "|ID_CHECKBOX_Spirals1_Blend|E1_CHECKBOX_Spirals_Blend";
    replace_str = replace_str + "|ID_CHECKBOX_Spirals2_3D|E2_CHECKBOX_Spirals_3D";
    replace_str = replace_str + "|ID_CHECKBOX_Spirals2_Blend|E2_CHECKBOX_Spirals_Blend";
    replace_str = replace_str + "|ID_CHECKBOX_Spirograph1_Animate|E1_CHECKBOX_Spirograph_Animate";
    replace_str = replace_str + "|ID_CHECKBOX_Spirograph2_Animate|E2_CHECKBOX_Spirograph_Animate";
    replace_str = replace_str + "|ID_CHECKBOX_Twinkle1_Strobe|E1_CHECKBOX_Twinkle_Strobe";
    replace_str = replace_str + "|ID_CHECKBOX_Twinkle2_Strobe|E2_CHECKBOX_Twinkle_Strobe";
    replace_str = replace_str + "|ID_CHOICE_Bars1_Direction|E1_CHOICE_Bars_Direction";
    replace_str = replace_str + "|ID_CHOICE_Bars2_Direction|E2_CHOICE_Bars_Direction";
    replace_str = replace_str + "|ID_CHOICE_Butterfly1_Colors|E1_CHOICE_Butterfly_Colors";
    replace_str = replace_str + "|ID_CHOICE_Butterfly2_Colors|E2_CHOICE_Butterfly_Colors";
    replace_str = replace_str + "|ID_CHOICE_LayerMethod|ID_CHOICE_LayerMethod";
    replace_str = replace_str + "|ID_CHOICE_Meteors1_Effect|E1_CHOICE_Meteors_Effect";
    replace_str = replace_str + "|ID_CHOICE_Meteors1_Type|E1_CHOICE_Meteors_Type";
    replace_str = replace_str + "|ID_CHOICE_Meteors2_Effect|E2_CHOICE_Meteors_Effect";
    replace_str = replace_str + "|ID_CHOICE_Meteors2_Type|E2_CHOICE_Meteors_Type";
    replace_str = replace_str + "|ID_CHOICE_Pictures1_Direction|E1_CHOICE_Pictures_Direction";
    replace_str = replace_str + "|ID_CHOICE_Pictures2_Direction|E2_CHOICE_Pictures_Direction";

    replace_str = replace_str + "|ID_SLIDER_Bars1_BarCount|E1_SLIDER_Bars_BarCount";
    replace_str = replace_str + "|ID_SLIDER_Bars2_BarCount|E2_SLIDER_Bars_BarCount";
    replace_str = replace_str + "|ID_SLIDER_Brightness|ID_SLIDER_Brightness";
    replace_str = replace_str + "|ID_SLIDER_Butterfly1_Chunks|E1_SLIDER_Butterfly_Chunks";
    replace_str = replace_str + "|ID_SLIDER_Butterfly1_Skip|E1_SLIDER_Butterfly_Skip";
    replace_str = replace_str + "|ID_SLIDER_Butterfly1_Style|E1_SLIDER_Butterfly_Style";
    replace_str = replace_str + "|ID_SLIDER_Butterfly2_Chunks|E2_SLIDER_Butterfly_Chunks";
    replace_str = replace_str + "|ID_SLIDER_Butterfly2_Skip|E2_SLIDER_Butterfly_Skip";
    replace_str = replace_str + "|ID_SLIDER_Butterfly2_Style|E2_SLIDER_Butterfly_Style";
    replace_str = replace_str + "|ID_SLIDER_Circles1_Count|E1_SLIDER_Circles_Count";
    replace_str = replace_str + "|ID_SLIDER_Circles1_Size|E1_SLIDER_Circles_Size";
    replace_str = replace_str + "|ID_SLIDER_Circles2|E2_SLIDER_Circles";
    replace_str = replace_str + "|ID_SLIDER_Circles2_count|E2_SLIDER_Circles_count";
    replace_str = replace_str + "|ID_SLIDER_ColorWash1_Count|E1_SLIDER_ColorWash_Count";
    replace_str = replace_str + "|ID_SLIDER_ColorWash2_Count|E2_SLIDER_ColorWash_Count";
    replace_str = replace_str + "|ID_SLIDER_Contrast|ID_SLIDER_Contrast";
    replace_str = replace_str + "|ID_SLIDER_Fire1_Height|E1_SLIDER_Fire_Height";
    replace_str = replace_str + "|ID_SLIDER_Fire1_HueShift|E1_SLIDER_Fire_HueShift";
    replace_str = replace_str + "|ID_SLIDER_Fire2_Height|E2_SLIDER_Fire_Height";
    replace_str = replace_str + "|ID_SLIDER_Fire2_HueShift|E2_SLIDER_Fire_HueShift";
    replace_str = replace_str + "|ID_SLIDER_Fireworks1_Count|E1_SLIDER_Fireworks_Count";
    replace_str = replace_str + "|ID_SLIDER_Fireworks1_Fade|E1_SLIDER_Fireworks_Fade";
    replace_str = replace_str + "|ID_SLIDER_Fireworks1_Number_Explosions|E1_SLIDER_Fireworks_Number_Explosions";
    replace_str = replace_str + "|ID_SLIDER_Fireworks1_Velocity|E1_SLIDER_Fireworks_Velocity";
    replace_str = replace_str + "|ID_SLIDER_Fireworks2_Count|E2_SLIDER_Fireworks_Count";
    replace_str = replace_str + "|ID_SLIDER_Fireworks2_Fade|E2_SLIDER_Fireworks_Fade";
    replace_str = replace_str + "|ID_SLIDER_Fireworks2_Number_Explosions|E2_SLIDER_Fireworks_Number_Explosions";
    replace_str = replace_str + "|ID_SLIDER_Fireworks2_Velocity|E2_SLIDER_Fireworks_Velocity";
    replace_str = replace_str + "|ID_SLIDER_Garlands1_Spacing|E1_SLIDER_Garlands_Spacing";
    replace_str = replace_str + "|ID_SLIDER_Garlands1_Type|E1_SLIDER_Garlands_Type";
    replace_str = replace_str + "|ID_SLIDER_Garlands2_Spacing|E2_SLIDER_Garlands_Spacing";
    replace_str = replace_str + "|ID_SLIDER_Garlands2_Type|E2_SLIDER_Garlands_Type";
    replace_str = replace_str + "|ID_SLIDER_Life1_Count|E1_SLIDER_Life_Count";
    replace_str = replace_str + "|ID_SLIDER_Life1_Seed|E1_SLIDER_Life_Seed";
    replace_str = replace_str + "|ID_SLIDER_Life2_Count|E2_SLIDER_Life_Count";
    replace_str = replace_str + "|ID_SLIDER_Life2_Seed|E2_SLIDER_Life_Seed";
    replace_str = replace_str + "|ID_SLIDER_Meteors1_Count|E1_SLIDER_Meteors_Count";
    replace_str = replace_str + "|ID_SLIDER_Meteors1_Length|E1_SLIDER_Meteors_Length";
    replace_str = replace_str + "|ID_SLIDER_Meteors1_Swirl_Intensity|E1_SLIDER_Meteors_Swirl_Intensity";
    replace_str = replace_str + "|ID_SLIDER_Meteors2_Count|E2_SLIDER_Meteors_Count";
    replace_str = replace_str + "|ID_SLIDER_Meteors2_Length|E2_SLIDER_Meteors_Length";
    replace_str = replace_str + "|ID_SLIDER_Meteors2_Swirl_Intensity|E2_SLIDER_Meteors_Swirl_Intensity";
    replace_str = replace_str + "|ID_SLIDER_Piano1_Keyboard|E1_SLIDER_Piano_Keyboard";
    replace_str = replace_str + "|ID_SLIDER_Piano2_Keyboard|E2_SLIDER_Piano_Keyboard";
    replace_str = replace_str + "|ID_SLIDER_Pictures1_GifSpeed|E1_SLIDER_Pictures_GifSpeed";
    replace_str = replace_str + "|ID_SLIDER_Pictures2_GifSpeed|E2_SLIDER_Pictures_GifSpeed";
    replace_str = replace_str + "|ID_SLIDER_Snowflakes1_Count|E1_SLIDER_Snowflakes_Count";
    replace_str = replace_str + "|ID_SLIDER_Snowflakes1_Type|E1_SLIDER_Snowflakes_Type";
    replace_str = replace_str + "|ID_SLIDER_Snowflakes2_Count|E2_SLIDER_Snowflakes_Count";
    replace_str = replace_str + "|ID_SLIDER_Snowflakes2_Type|E2_SLIDER_Snowflakes_Type";
    replace_str = replace_str + "|ID_SLIDER_Snowstorm1_Count|E1_SLIDER_Snowstorm_Count";
    replace_str = replace_str + "|ID_SLIDER_Snowstorm1_Length|E1_SLIDER_Snowstorm_Length";
    replace_str = replace_str + "|ID_SLIDER_Snowstorm2_Count|E2_SLIDER_Snowstorm_Count";
    replace_str = replace_str + "|ID_SLIDER_Snowstorm2_Length|E2_SLIDER_Snowstorm_Length";
    replace_str = replace_str + "|ID_SLIDER_SparkleFrequency|ID_SLIDER_SparkleFrequency";
    replace_str = replace_str + "|ID_SLIDER_Speed1|E1_SLIDER_Speed";
    replace_str = replace_str + "|ID_SLIDER_Speed2|E2_SLIDER_Speed";
    replace_str = replace_str + "|ID_SLIDER_Spirals1_Count|E1_SLIDER_Spirals_Count";
    replace_str = replace_str + "|ID_SLIDER_Spirals1_Direction|E1_SLIDER_Spirals_Direction";
    replace_str = replace_str + "|ID_SLIDER_Spirals1_Rotation|E1_SLIDER_Spirals_Rotation";
    replace_str = replace_str + "|ID_SLIDER_Spirals1_Thickness|E1_SLIDER_Spirals_Thickness";
    replace_str = replace_str + "|ID_SLIDER_Spirals2_Count|E2_SLIDER_Spirals_Count";
    replace_str = replace_str + "|ID_SLIDER_Spirals2_Direction|E2_SLIDER_Spirals_Direction";
    replace_str = replace_str + "|ID_SLIDER_Spirals2_Rotation|E2_SLIDER_Spirals_Rotation";
    replace_str = replace_str + "|ID_SLIDER_Spirals2_Thickness|E2_SLIDER_Spirals_Thickness";
    replace_str = replace_str + "|ID_SLIDER_Spirograph1_d|E1_SLIDER_Spirograph_d";
    replace_str = replace_str + "|ID_SLIDER_Spirograph1_R|E1_SLIDER_Spirograph_R";
    replace_str = replace_str + "|ID_SLIDER_Spirograph1_r|E1_SLIDER_Spirograph_r";
    replace_str = replace_str + "|ID_SLIDER_Spirograph2_d|E2_SLIDER_Spirograph_d";
    replace_str = replace_str + "|ID_SLIDER_Spirograph2_R|E2_SLIDER_Spirograph_R";
    replace_str = replace_str + "|ID_SLIDER_Spirograph2_r|E2_SLIDER_Spirograph_r";

    replace_str = replace_str + "|ID_SLIDER_Tree1_Branches|E1_SLIDER_Tree_Branches";
    replace_str = replace_str + "|ID_SLIDER_Tree2_Branches|E2_SLIDER_Tree_Branches";
    replace_str = replace_str + "|ID_SLIDER_Twinkle1_Count|E1_SLIDER_Twinkle_Count";
    replace_str = replace_str + "|ID_SLIDER_Twinkle1_Steps|E1_SLIDER_Twinkle_Steps";
    replace_str = replace_str + "|ID_SLIDER_Twinkle2_Count|E2_SLIDER_Twinkle_Count";
    replace_str = replace_str + "|ID_SLIDER_Twinkle2_Steps|E2_SLIDER_Twinkle_Steps";
    replace_str = replace_str + "|ID_TEXTCTRL_Pictures1_Filename|E1_TEXTCTRL_Pictures_Filename";
    replace_str = replace_str + "|ID_TEXTCTRL_Pictures2_Filename|E2_TEXTCTRL_Pictures_Filename";


    Text1 = Text1 + "|ID_TEXTCTRL_Text1_1_Font|ID_TEXTCTRL_Text1_1_Font=";
    Text2 = Text2 + "|ID_TEXTCTRL_Text2_1_Font|ID_TEXTCTRL_Text2_1_Font=";
    Text1 = Text1 + "|ID_TEXTCTRL_Text1_2_Font|ID_TEXTCTRL_Text1_2_Font=";
    Text2 = Text2 + "|ID_TEXTCTRL_Text2_2_Font|ID_TEXTCTRL_Text2_2_Font=";
    Text1 = Text1 + "|ID_CHOICE_Text1_1_Dir|ID_CHOICE_Text1_1_Dir=left";
    Text2 = Text2 + "|ID_CHOICE_Text2_1_Dir|ID_CHOICE_Text2_1_Dir=left";
    Text1 = Text1 + "|ID_CHOICE_Text1_2_Dir|ID_CHOICE_Text1_2_Dir=left";
    Text2 = Text2 + "|ID_CHOICE_Text2_2_Dir|ID_CHOICE_Text2_2_Dir=left";
    Text1 = Text1 + "|ID_SLIDER_Text1_1_Position|ID_SLIDER_Text1_1_Position=50";
    Text2 = Text2 + "|ID_SLIDER_Text2_1_Position|ID_SLIDER_Text2_1_Position=50";
    Text1 = Text1 + "|ID_SLIDER_Text1_2_Position|ID_SLIDER_Text1_2_Position=50";
    Text2 = Text2 + "|ID_SLIDER_Text2_2_Position|ID_SLIDER_Text2_2_Position=50";
    Text1 = Text1 + "|ID_SLIDER_Text1_1_TextRotation|ID_SLIDER_Text1_1_TextRotation=0";
    Text2 = Text2 + "|ID_SLIDER_Text2_1_TextRotation|ID_SLIDER_Text2_1_TextRotation=0";
    Text1 = Text1 + "|ID_SLIDER_Text1_2_TextRotation|ID_SLIDER_Text1_2_TextRotation=0";
    Text2 = Text2 + "|ID_SLIDER_Text2_2_TextRotation|ID_SLIDER_Text2_2_TextRotation=0";
    Text1 = Text1 + "|ID_CHECKBOX_Text1_COUNTDOWN1|ID_CHECKBOX_Text1_COUNTDOWN1=0";
    Text2 = Text2 + "|ID_CHECKBOX_Text2_COUNTDOWN1|ID_CHECKBOX_Text2_COUNTDOWN1=0";
    Text1 = Text1 + "|ID_CHECKBOX_Text1_COUNTDOWN2|ID_CHECKBOX_Text1_COUNTDOWN2=0";
    Text2 = Text2 + "|ID_CHECKBOX_Text2_COUNTDOWN2|ID_CHECKBOX_Text2_COUNTDOWN2=0";

    replace_str = replace_str + "|ID_TEXTCTRL_Text1_1_Font|E1_TEXTCTRL_Text_Font1";
    replace_str = replace_str + "|ID_TEXTCTRL_Text1_2_Font|E1_TEXTCTRL_Text_Font2";
    replace_str = replace_str + "|ID_TEXTCTRL_Text2_1_Font|E2_TEXTCTRL_Text_Font1";
    replace_str = replace_str + "|ID_TEXTCTRL_Text2_2_Font|E2_TEXTCTRL_Text_Font2";
    replace_str = replace_str + "|ID_TEXTCTRL_Text1_Line1|E1_TEXTCTRL_Text_Line1";
    replace_str = replace_str + "|ID_TEXTCTRL_Text1_Line2|E1_TEXTCTRL_Text_Line2";
    replace_str = replace_str + "|ID_TEXTCTRL_Text2_Line1|E2_TEXTCTRL_Text_Line1";
    replace_str = replace_str + "|ID_TEXTCTRL_Text2_Line2|E2_TEXTCTRL_Text_Line2";
    replace_str = replace_str + "|ID_SLIDER_Text1_1_Position|E1_SLIDER_Text_Position1";
    replace_str = replace_str + "|ID_SLIDER_Text1_2_Position|E1_SLIDER_Text_Position2";
    replace_str = replace_str + "|ID_SLIDER_Text2_1_Position|E2_SLIDER_Text_Position1";
    replace_str = replace_str + "|ID_SLIDER_Text2_2_Position|E2_SLIDER_Text_Position2";
    replace_str = replace_str + "|ID_CHOICE_Text1_1_Count|E1_CHOICE_Text_Count1";
    replace_str = replace_str + "|ID_CHOICE_Text1_2_Count|E1_CHOICE_Text_Count2";
    replace_str = replace_str + "|ID_CHOICE_Text2_1_Count|E2_CHOICE_Text_Count1";
    replace_str = replace_str + "|ID_CHOICE_Text2_2_Count|E2_CHOICE_Text_Count2";
    replace_str = replace_str + "|ID_CHOICE_Text1_1_Dir|E1_CHOICE_Text_Dir1";
    replace_str = replace_str + "|ID_CHOICE_Text1_2_Dir|E1_CHOICE_Text_Dir2";
    replace_str = replace_str + "|ID_CHOICE_Text2_1_Dir|E2_CHOICE_Text_Dir1";
    replace_str = replace_str + "|ID_CHOICE_Text2_2_Dir|E2_CHOICE_Text_Dir2";
    replace_str = replace_str + "|ID_CHOICE_Text1_1_Effect|E1_CHOICE_Text_Effect1";
    replace_str = replace_str + "|ID_CHOICE_Text1_2_Effect|E1_CHOICE_Text_Effect2";
    replace_str = replace_str + "|ID_CHOICE_Text2_1_Effect|E2_CHOICE_Text_Effect1";
    replace_str = replace_str + "|ID_CHOICE_Text2_2_Effect|E2_CHOICE_Text_Effect2";
    //
    //
    replace_str = replace_str + "|ID_CHECKBOX_Meteors1_FallUp|E1_ID_CHECKBOX_Meteors1_FallUp";
    replace_str = replace_str + "|ID_CHECKBOX_Meteors2_FallUp|E2_ID_CHECKBOX_Meteors1_FallUp";

//  single strand effects
    replace_str = replace_str + "|E1_SLIDER_Single_Color_Mix1|ID_SLIDER_Single_Color_Mix1";
    replace_str = replace_str + "|E1_SLIDER_Single_Color_Spacing1|ID_SLIDER_Chase_Spacing1";
    replace_str = replace_str + "|E1_CHECKBOX_Single_Chase_3dFade1|ID_CHECKBOX_Chase_3dFade1";
    replace_str = replace_str + "|E2_SLIDER_Single_Color_Mix2|ID_SLIDER_Single_Color_Mix2";
    replace_str = replace_str + "|E2_SLIDER_Single_Color_Spacing2|ID_SLIDER_Chase_Spacing2";
    replace_str = replace_str + "|E2_CHECKBOX_Single_Group_Arches2|ID_CHECKBOX_Group_Arches2";

    replace_str = replace_str + "|ID_SLIDER_Single_Color_Mix1|E1_SLIDER_Color_Mix1";
    replace_str = replace_str + "|ID_SLIDER_Chase_Spacing1|E1_SLIDER_Chase_Spacing1";
    replace_str = replace_str + "|ID_CHECKBOX_Group_Arches1|E1_CHECKBOX_Chase_3dFade1";

//  Single Strand
    replace_str = replace_str + "|E1_CHECKBOX_R_TO_L1|E1_CHOICE_Chase_Type1=Right-Left";

//  RIPPLE
 replace_str = replace_str + "|E1_SLIDER5|E1_SLIDER_Ripple_Thickness";
 replace_str = replace_str + "|E2_SLIDER5|E2_SLIDER_Ripple_Thickness";
 replace_str = replace_str + "|E1_CHECKBOX3|E1_CHECKBOX_Ripple3D";
 replace_str = replace_str + "|E2_CHECKBOX3|E2_CHECKBOX_Ripple3D";


/*
E2_CHOICE_Ripple_Movement=Implode,
E2_SLIDER_Ripple_Thickness=36,
E2_SLIDER5=0, // ID_SLIDER_Ripple_Thickness
E2_CHECKBOX_Ripple_Blend=1,
E2_CHECKBOX3=1, // ID_CHECKBOX_Ripple3D
*/

    //    replace_str = replace_str + "|ID_CHECKBOX_Meteors1_FallUp|E1_CHECKBOX_Meteors_FallUp";
//    replace_str = replace_str + "|ID_CHECKBOX_Meteors2_FallUp|E2_CHECKBOX_Meteors_FallUp";
//

//    E1_TEXTCTRL_Text_Font1=
//	,E1_CHOICE_Text_Dir1=left
//	,E1_CHOICE_Text_Effect1=normal
//	,E1_CHOICE_Text_Count1=none
//	,E1_SLIDER_Text_Position1=50
//	,E1_TEXTCTRL_Text_Font2=
//	,E1_CHOICE_Text_Dir2=left
//	,E1_CHOICE_Text_Effect2=normal
//	,E1_CHOICE_Text_Count2=none
//	,E1_SLIDER_Text_Position2=50
//	,E1_BUTTON_Palette1=#FF0000
//	,E1_CHECKBOX_Palette1=0
//	,E1_BUTTON_Palette2=#00FF00
//	,E1_CHECKBOX_Palette2=0
//	,E1_BUTTON_Palette3=#0000FF
//	,E1_CHECKBOX_Palette3=1
//	,E1_BUTTON_Palette4=#FFFF00
//	,E1_CHECKBOX_Palette4=0
//	,E1_BUTTON_Palette5=#FFFFFF
//	,E1_CHECKBOX_Palette5=0
//	,E1_BUTTON_Palette6=#000000
//	,E1_CHECKBOX_Palette6=1


    //  this set will convert old, unsed tokens into a new not used token. this eliminates the error messages
    replace_str = replace_str + "|ID_SLIDER_Text1_1_TextRotation|E1_SLIDER_Text_Rotation1";
    replace_str = replace_str + "|ID_SLIDER_Text1_2_TextRotation|E1_SLIDER_Text_Rotation2";
    replace_str = replace_str + "|ID_SLIDER_Text2_1_TextRotation|E2_SLIDER_Text_Rotation1";
    replace_str = replace_str + "|ID_SLIDER_Text2_2_TextRotation|E2_SLIDER_Text_Rotation2";
    replace_str = replace_str + "|ID_CHECKBOX_Text1_COUNTDOWN1|E1_Text1_COUNTDOWN";
    replace_str = replace_str + "|ID_CHECKBOX_Text2_COUNTDOWN1|E1_Text2_COUNTDOWN";
    replace_str = replace_str + "|ID_CHECKBOX_Text1_COUNTDOWN2|E2_Text1_COUNTDOWN";
    replace_str = replace_str + "|ID_CHECKBOX_Text2_COUNTDOWN2|E2_Text2_COUNTDOWN";
    replace_str = replace_str + "|ID_SLIDER_Text_Rotation1|E1_SLIDER_Text_Rotation";
    replace_str = replace_str + "|ID_SLIDER_Text_Rotation2|E2_SLIDER_Text_Rotation";
    replace_str = replace_str + "|ID_Text1_Countdown|E1_Text_Countdown";
    replace_str = replace_str + "|ID_Text2_Countdown|E2_Text_Countdown";
    replace_str = replace_str + "|ID_Text1_COUNTDOWN|E1_Text_COUNTDOWN";
    replace_str = replace_str + "|ID_Text2_COUNTDOWN|E2_Text_COUNTDOWN";

    replace_str = replace_str + "|ID_TEXTCTRL_Effect1_Fadein|E1_TEXTCTRL_Fadein";
    replace_str = replace_str + "|ID_TEXTCTRL_Effect1_Fadeout|E1_TEXTCTRL_Fadeout";
    replace_str = replace_str + "|ID_TEXTCTRL_Effect2_Fadein|E2_TEXTCTRL_Fadein";
    replace_str = replace_str + "|ID_TEXTCTRL_Effect2_Fadeout|E2_TEXTCTRL_Fadeout";
    replace_str = replace_str + "|ID_CHECKBOX_Effect1_Fit|E1_CHECKBOX_FitToTime";
    replace_str = replace_str + "|ID_CHECKBOX_Effect2_Fit|E2_CHECKBOX_FitToTime";

    replace_str = replace_str + "|vertical text up|vert text up";
    replace_str = replace_str + "|vertical dext down|vert text down";
    replace_str = replace_str + "|count down seconds|seconds";
    replace_str = replace_str + "|count down to date|to date";

    if (!f.Create(fileout,true))
    {
        return;
    }

    tfile.Open(file); // open input file
// read the first line
    str =  tfile.GetFirstLine() + "\n";


    f.Write(str);
    int pos,pos_SLIDER_Slider,pos_ID_TEXTCTRL4;

// read all lines one by one
// until the end of the file
    while(!tfile.Eof())
    {
        str = tfile.GetNextLine();

        pos=str.find("ID_SLIDER",0); // is this line a Effect?
        if(pos>0) // are we on the xml line containg the effect?
        {
            //  Yes

            //  do we have the bad SILDER_slider token?
            pos_SLIDER_Slider=str.find("SLIDER_Slider",0);
            if(pos_SLIDER_Slider>0) // if we have SLIDER_Slider bad text,
            {
                modified=true;  // yes,fix it
                str.Replace("SLIDER_Slider","SLIDER");
            }

// do we have the old text1 font token?
            pos_ID_TEXTCTRL4=str.find("ID_TEXTCTRL4",0);
            if(pos_ID_TEXTCTRL4>0) // if we have ID_TEXTCTRL4 bad text,
            {
                modified=true;  // yes,fix it
                str.Replace("ID_TEXTCTRL4","ID_TEXTCTRL_Text1_1_Font");
            }

//  166 tokens
            modified=true;
            str=InsertMissing(str,replace_str,false);
            str=InsertMissing(str,missing,true);

//  now look to fill in any missing tokens

            /* comment out now with ver 25
                   p=str.find("ID_SLIDER",0);
                   if(p>0) // Look for lines that should have brightness and contrast, in other words all
                   {
                       modified=true;
                   }

                   p=str.find("ID_TEXTCTRL_Text1_Line1",0);
                   if(p>0) // Is this a text 1 line?
                   {
                       modified=true;
                       str=InsertMissing(str,Text1,true);
                   }
                   p=str.find("ID_TEXTCTRL_Text2_Line1",0);
                   if(p>0) // is this a text 2 line?
                   {
                       modified=true;
                       str=InsertMissing(str,Text2,true);
                   }

                   p=str.find("ID_CHOICE_Meteors1",0);
                   if(p>0) // is there a meteors 1 effect on this line?
                   {
                       modified=true;
                       str=InsertMissing(str,Meteors1,true); // fix any missing values
                   }
                   p=str.find("ID_CHOICE_Meteors2",0);
                   if(p>0) // is there a meteors 1 effect on this line?
                   {
                       modified=true;
                       str=InsertMissing(str,Meteors2,true);
                   }

                   p=str.find("ID_CHOICE_Fire11",0);
                   if(p>0) // is there a meteors 1 effect on this line?
                   {
                       modified=true;
                       str=InsertMissing(str,Fire1,true); // fix any missing values
                   }
                   p=str.find("ID_CHOICE_Fire2",0);
                   if(p>0) // is there a meteors 1 effect on this line?
                   {
                       modified=true;
                       str=InsertMissing(str,Fire2,true); // fix any missing values
                   }
                     */

        }
        str = str + "\n";
        f.Write(str); // placeholder, do whatever you want with the string

    }
    tfile.Close();
    f.Close();
    if(modified) wxCopyFile(fileout,file,true); // if we modified the file, copy over it
    wxRemoveFile(fileout); // get rid of temporary file
}

//void djdebug(const char* fmt, ...); //_DJ
void xLightsFrame::ProcessAudacityTimingFile(const wxString& filename)
{
    wxTextFile f;
    wxString line;

    int r;

    if (!f.Open(filename.c_str()))
    {
        //Add error dialog if open file failed
        return;
    }

    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    for(r=0, line = f.GetFirstLine(); !f.Eof(); line = f.GetNextLine(), r++)
    {
        std::string::size_type ofs;
        if ((ofs = line.find("#")) != std::string::npos) line.erase(ofs); //remove comments
//        while (!linebuf.empty() && (linebuf.back() == '\\')) //line continuation
//        {
//            linebuf.pop_back(); //remove trailing "\"
//            std::string morebuf;
//            if (!std::getline(infile.stream, morebuf)) break;
//            linebuf += morebuf;
//        }
        while (!line.empty() && (line.Last() == ' ')) line.RemoveLast(); //trim trailing spaces
        if (line.empty())
        {
            --r;    //skip blank lines; don't add grid row
            continue;
        }

        wxStringTokenizer tkz(line, "\t");
        wxString token = tkz.GetNextToken(); //first column = start time
#if 1 //pull in lyrics or other label text -DJ
        tkz.GetNextToken(); //second column = end time; ignored by Nutcracker
        wxString label = tkz.GetNextToken(); //third column = label/text -DJ
        for (;;) //collect remaining tokens into label
        {
            wxString more = tkz.GetNextToken();
            if (more.empty()) break;
            label += " " + more;
        }
#endif // 1
        Grid1->AppendRows();
//        djdebug("line last token = '%s', t2 = %s, t3 %s", (const char*)token, (const char*)token2, (const char*)token3); //-DJ
        Grid1->SetCellValue(r, 0, token);
#if 1
        Grid1->SetCellValue(r, 1, label); //add label text -DJ
#endif // 1
    }
}

void xLightsFrame::ImportAudacityTimings()
{
    wxFileDialog* OpenDialog = new wxFileDialog(
        this, _("Choose Audacity timing file"), CurrentDir, wxEmptyString,
        _("Text files (*.txt)|*.txt"),		wxFD_OPEN, wxDefaultPosition);
    wxString fName;
    if (OpenDialog->ShowModal() == wxID_OK)
    {
        fName =	OpenDialog->GetPath();
        ProcessAudacityTimingFile(fName);
    }
    else
    {

    }

    // Clean up after ourselves
    OpenDialog->Destroy();
}

void xLightsFrame::ProcessxLightsXMLTimingsFile(const wxString& filename)
{

}

void xLightsFrame::ImportxLightsXMLTimings()
{
    wxFileDialog OpenDialog(this, _("Choose xLights sequence"), CurrentDir, wxEmptyString,
                            _("Text files (*.xml)|*.xml"),wxFD_OPEN, wxDefaultPosition);
    wxString fName;

    if (OpenDialog.ShowModal() == wxID_OK)
    {
        fName =	OpenDialog.GetPath();
        SeqLoadXlightsFile(fName, true);
    }
}

// load the specified .xseq binary file
void xLightsFrame::SeqLoadXlightsXSEQ(const wxString& filename)
{
    // read xlights file
    ReadXlightsFile(filename);
    DisplayXlightsFilename(filename);
    SeqBaseChannel=1;
    SeqChanCtrlBasic=false;
    SeqChanCtrlColor=false;
}

// Load the xml file containing effects for a particular sequence
// Returns true if file exists and was read successfully
bool xLightsFrame::SeqLoadXlightsFile(const wxString& filename, bool ChooseModels)
{
    wxString tmpStr;
    // read xml sequence info
    wxFileName FileObj(filename);
    FileObj.SetExt("xml");
    SeqXmlFileName=FileObj.GetFullPath();
    int gridCol;
    if (!FileObj.FileExists())
    {
        if (ChooseModels) ChooseModelsForSequence();
        return false;
    }

    // read xml
    //  first fix any version specific changes
    FixVersionDifferences(SeqXmlFileName);


    wxXmlDocument doc;
    if (!doc.Load(SeqXmlFileName))
    {
        wxMessageBox(_("Error loading: ")+SeqXmlFileName);
        return false;
    }
    wxXmlNode* root=doc.GetRoot();
    wxString tempstr=root->GetAttribute("BaseChannel", "1");
    tempstr.ToLong(&SeqBaseChannel);
    tempstr=root->GetAttribute("ChanCtrlBasic", "0");
    SeqChanCtrlBasic=tempstr!="0";
    tempstr=root->GetAttribute("ChanCtrlColor", "0");
    SeqChanCtrlColor=tempstr!="0";

    wxXmlNode *tr, *td;
    wxString ColName;
    wxArrayInt DeleteCols;
    wxArrayString ModelNames;
    GetModelNames(ModelNames);
    SeqElementMismatchDialog dialog(this);
    dialog.ChoiceModels->Set(ModelNames);
    if (ModelNames.Count() > 0) dialog.ChoiceModels->SetSelection(0);
    int r,c; // row 0=heading, >=1 are data rows
    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    for(tr=root->GetChildren(), r=0; tr!=NULL; tr=tr->GetNext(), r++ )
    {
        if (tr->GetName() != "tr") continue;
        if (r > 0)
        {
            Grid1->AppendRows();
        }
        for(td=tr->GetChildren(), c=0, gridCol=0; td!=NULL; td=td->GetNext(), c++ )
        {
            if (td->GetName() != "td") continue;
            if (r==0)
            {
                if (c >= XLIGHTS_SEQ_STATIC_COLUMNS)
                {
                    ColName=td->GetNodeContent();
                    if (ModelNames.Index(ColName) == wxNOT_FOUND)
                    {
                        dialog.StaticTextMessage->SetLabel("Element '"+ColName+"'\ndoes not exist in your list of models");
                        dialog.Fit();
                        dialog.ShowModal();
                        if (dialog.RadioButtonAdd->GetValue())
                        {
                        }
                        else if (dialog.RadioButtonDelete->GetValue())
                        {
                            DeleteCols.Add(c);
                        }
                        else
                        {
                            // rename
                            Grid1->AppendCols();
                            Grid1->SetColLabelValue(Grid1->GetNumberCols()-1, dialog.ChoiceModels->GetStringSelection());
                        }
                    }
                    else
                    {
                        Grid1->AppendCols();
                        Grid1->SetColLabelValue(Grid1->GetNumberCols()-1, ColName);
                    }
                }
            }
            else if (DeleteCols.Index(c) == wxNOT_FOUND)
            {

                Grid1->SetCellValue(r-1,gridCol,td->GetNodeContent());
                if(td->GetAttribute("Protected",&tmpStr) && tmpStr == "1")
                {
                    Grid1->SetCellTextColour(r-1, gridCol, *wxBLUE);
                }
                Grid1->SetCellOverflow(r-1,gridCol, false);
                Grid1->SetReadOnly(r-1,gridCol,false);
                gridCol++; //c does not work here since it is following the columns in the input file not the columns in the grid

            }
        }
    }
    EnableSequenceControls(true);
    //Grid1->HideCol(Grid1->GetNumberCols()-1);
    changedRow = 99999;
    changedColumn = 99999;
    return true;
}

void xLightsFrame::ResetSequenceGrid()
{
    int n;
    n=Grid1->GetNumberCols();
    if (n > XLIGHTS_SEQ_STATIC_COLUMNS) Grid1->DeleteCols(XLIGHTS_SEQ_STATIC_COLUMNS, n-XLIGHTS_SEQ_STATIC_COLUMNS);
    n=Grid1->GetNumberRows();
    if (n > 0) Grid1->DeleteRows(0, n);
}

void xLightsFrame::OpenSequence()
{
    wxArrayString XSeqFiles,LorFiles,MediaFiles;

    // get list of media files
    wxFileName oName;
    wxString filename;
    wxString nullString;
    oName.AssignDir( CurrentDir );
    wxDir dir(CurrentDir);
    nullString.Clear();
    int interval=Timer1.GetInterval();

    if (UnsavedChanges && wxNO == wxMessageBox("Sequence changes will be lost.  Do you wish to continue?",
            "Sequence Changed Confirmation", wxICON_QUESTION | wxYES_NO))
    {
        return;
    }

    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    while ( cont )
    {
        oName.SetFullName(filename);
        switch (ExtType(oName.GetExt()))
        {
        case 'a':
        case 'v':
            MediaFiles.Add(oName.GetFullPath());
            break;
        case 'X':
            XSeqFiles.Add(oName.GetFullPath());
            break;
        case 'L':
            LorFiles.Add(oName.GetFullPath());
            break;
        }
        cont = dir.GetNext(&filename);
    }

    // populate dialog
    SeqOpenDialog dialog(this);
    if (XSeqFiles.Count() > 0)
    {
        dialog.ChoiceSeqFiles->Set(XSeqFiles);
        dialog.ChoiceSeqFiles->SetSelection(0);
    }
    else
    {
        dialog.RadioButtonXlights->Enable(false);
        dialog.ChoiceSeqFiles->Enable(false);
        dialog.RadioBoxTimingChoice->Enable();
        dialog.RadioButtonNewMusic->SetValue(true);
    }
    if (LorFiles.Count() > 0)
    {
        dialog.ChoiceLorFiles->Set(LorFiles);
        dialog.ChoiceLorFiles->SetSelection(0);
    }
    else
    {
        dialog.RadioButtonLor->Enable(false);
        dialog.ChoiceLorFiles->Enable(false);
    }
    if (MediaFiles.Count() > 0)
    {
        dialog.ChoiceMediaFiles->Set(MediaFiles);
        dialog.ChoiceMediaFiles->SetSelection(0);
    }
    else
    {
        dialog.RadioButtonNewMusic->Enable(false);
        dialog.ChoiceMediaFiles->Enable(false);
        dialog.RadioBoxTimingChoice->Disable();
        dialog.RadioButtonNewAnim->SetValue(true);
    }
    dialog.Fit();
    if (dialog.ShowModal() != wxID_OK) return;  // user pressed cancel

    mediaFilename.Clear();
    ResetSequenceGrid();
    ResetTimer(NO_SEQ);
    previewLoaded = false;
    previewPlaying = false;
    changedRow = 99999;
    changedColumn = 99999;

    long duration;
    if (dialog.RadioButtonXlights->GetValue())
    {
        wxStopWatch sw; // start a stopwatch timer
        SeqLoadXlightsXSEQ(dialog.ChoiceSeqFiles->GetStringSelection());
        SeqLoadXlightsFile(dialog.ChoiceSeqFiles->GetStringSelection(), true);
        float elapsedTime = sw.Time()/1000.0; //msec => sec
        StatusBar1->SetStatusText(wxString::Format(_("'%s' loaded in %4.3f sec."), dialog.ChoiceSeqFiles->GetStringSelection(), elapsedTime));
        return;
    }
    else if (dialog.RadioButtonLor->GetValue())
    {
        filename=dialog.ChoiceLorFiles->GetStringSelection();
        ReadLorFile(filename);
        oName.SetFullName( filename );
        oName.SetExt(_(XLIGHTS_SEQUENCE_EXT));
        DisplayXlightsFilename(oName.GetFullPath());
        oName.SetExt("xml");
        SeqXmlFileName=oName.GetFullPath();
        SeqBaseChannel=1;
        SeqChanCtrlBasic=false;
        SeqChanCtrlColor=false;
        bool xmlFileLoaded=SeqLoadXlightsFile(filename, true);
        if (!xmlFileLoaded)
        {
            // No xml file, so put LOR timing into grid
            Grid1->AppendRows(LorTimingList.size());
            int r=0;
            for (std::set<int>::iterator it=LorTimingList.begin(); it != LorTimingList.end(); ++it)
            {
                int period=*it;
                float seconds=(float)period*interval/1000.0;
                Grid1->SetCellValue(r, 0, wxString::Format("%5.3f",seconds));
                r++;
            }
            wxMessageBox("Created new grid based on LOR effect timing");
        }
        return;
    }
    else if (dialog.RadioButtonNewMusic->GetValue())
    {
        UnsavedChanges = true;

        // determine media file length
        mediaFilename=dialog.ChoiceMediaFiles->GetStringSelection();

        DisplayXlightsFilename(nullString);

        if (!PlayerDlg->Play(mediaFilename))
        {
            wxMessageBox("Unable to load:\n"+mediaFilename,"ERROR");
            return;
        }
        for (int cnt=0; cnt < 50; cnt++)
        {
            duration=PlayerDlg->MediaCtrl->Length();  // milliseconds
            if (duration > 0) break;
            wxYield();
            wxMilliSleep(100);
        }
        PlayerDlg->MediaCtrl->Stop();
        if (duration <= 0)
        {
            wxMessageBox("Unable to determine the length of:\n"+mediaFilename,"ERROR");
            return;
        }
        switch (dialog.RadioBoxTimingChoice->GetSelection())
        {
        case 0:
            // No Timing mark import selected
            break;
        case 1:
            // Audacity File import
            ImportAudacityTimings();
            break;
        case 2:
            // Xlights XML timing import
            ImportxLightsXMLTimings();
            break;
        }
        //Clear xlights filename

    }
    else if (dialog.RadioButtonNewAnim->GetValue())
    {
        UnsavedChanges = true;
        // duration=dialog.SpinCtrlDuration->GetValue();  // seconds
        duration=0;
        wxString s_duration = dialog.SpinCtrlDuration_Float->GetValue(); // <SCM>
        double f_duration;
        if(!s_duration.ToDouble(&f_duration))
        {
            /* error! */
        }

        DisplayXlightsFilename(nullString);
        if (f_duration <= 0.0)
        {
            wxMessageBox("Invalid value for duration. Value must be > 0.001","ERROR");
            return;
        }
        //     duration*=1000;  // convert to milliseconds
        duration=f_duration*1000;  // convert to milliseconds <SCM>
    }

    SeqData.clear();
    SeqNumChannels=NetInfo.GetTotChannels();
    SeqNumPeriods=duration / interval;
    SeqDataLen=SeqNumPeriods * SeqNumChannels;
    SeqData.resize(SeqDataLen,0);
    int nSeconds=duration/1000;
    int nMinutes=nSeconds/60;
    nSeconds%=60;
    wxMessageBox(wxString::Format("Created empty sequence:\nChannels: %ld\nPeriods: %ld\nEach period is: %d msec\nTotal time: %d:%02d",
                                  SeqNumChannels,SeqNumPeriods,interval,nMinutes,nSeconds));
}

void xLightsFrame::OnBitmapButtonOpenSeqClick(wxCommandEvent& event)
{
    OpenSequence();
}

const wxString& DefaultAs(const wxString& str, const wxString& defval)
{
    return !str.IsEmpty()? str: defval;
}

class xLightsRenderThread : public wxThread
{
public:
    xLightsRenderThread(int col, const wxString &name, wxXmlNode *node, int numPeriods,
                        xLightsFrame *f, xLightsRenderThread ** thr)
    {
        myCol = col;
        completed = -1;
        prevCompleted = -1;
        ModelNode = node;
        buffer.InitBuffer(ModelNode);
        seqNumPeriods = numPeriods;
        xLights = f;
        threads = thr;
        ColName = name;
        firstRow = 0;
    }
    void AddEffectString(long time, const wxString &ef)
    {
//        debug(1, "aff eff str: time %ld, str '%s'", time, (const char*)ef.c_str());
        startTimes.push_back(time);
        effects.push_back(ef);
    }
    void setFirstRowToRender(int i)
    {
        firstRow = i;
    }
    void SetPreviousColCompleted(int i)
    {
        wxCriticalSection crit;
        crit.Enter();
        prevCompleted = i;
        crit.Leave();
    }
    ExitCode Entry ()
    {
        bool bufferClear = false;
        MapStringString SettingsMap;
        xLights->LoadSettingsMap("None,None,Effect 1", SettingsMap);

        int NextGridRowToPlay=firstRow;
        long msec = 0;
        for (int p=0; p<seqNumPeriods; p++)
        {
            msec=p * XTIMER_INTERVAL;

            if (!bufferClear)
            {
                wxString effect1=SettingsMap["E1_Effect"];
                wxString effect2=SettingsMap["E1_Effect"];
                int persist1=wxAtoi(SettingsMap["E1_CHECKBOX_OverlayBkg"]); //NOTE: no SettingsMap for this value first time thru loop
                int persist2=wxAtoi(SettingsMap["E2_CHECKBOX_OverlayBkg"]);

                if (!persist1 || "None" == effect1)
                {
                    bufferClear = true;
                    buffer.Clear(0); //allow effects to overlay onto other effects (useful for composite models) -DJ
                }
                if (!persist2 || "None" == effect2)
                {
                    buffer.Clear(1); //allow effects to overlay onto other effects (useful for composite models) -DJ
                }
                else
                {
                    bufferClear = false;
                }
            }
//            debug(10, "render loop: next grid row %d, #effects, msec %ld, next start time %ld", NextGridRowToPlay, effects.size(), msec, startTimes[NextGridRowToPlay]);
            if (NextGridRowToPlay < effects.size() && msec >= startTimes[NextGridRowToPlay])
            {
                wxCriticalSection crit;
                crit.Enter();
                int prev = prevCompleted;
                crit.Leave();

                while (NextGridRowToPlay > prev)
                {
                    //spin until we can continue, should replace with a semaphore or something
                    wxMilliSleep(2);
                    crit.Enter();
                    prev = prevCompleted;
                    crit.Leave();
                }
                while (NextGridRowToPlay > (completed + 1))
                {
                    completed++;
                    if (threads[myCol + 1] != NULL)
                    {
                        threads[myCol + 1]->SetPreviousColCompleted(completed);
                    }
                }
                // start next effect

                wxString EffectStr=effects[NextGridRowToPlay];
                EffectStr.Trim();
                if (!EffectStr.IsEmpty())
                {
                    wxString msg=_(wxString::Format("%s: Saving row %d/%d",ColName,NextGridRowToPlay+1,effects.size()));
                    if (onMainThread)
                    {
                        xLights->SetStatusText(msg);
                        wxYield();
                    }
                    else
                    {
                        thread1Mutex.Lock();
                        renderMessages.push_back(msg);
                        thread1Condition.Broadcast();
                        thread1Mutex.Unlock();
                        thread1Condition.Signal();
                    }

//debug(1, "at %d msec load settings for next row %d from '%s'", msec, NextGridRowToPlay, (const char*)effects[NextGridRowToPlay].c_str());
                    //If the new cell is empty we will let the state variable keep ticking so that effects do not jump
                    xLights->LoadSettingsMap(effects[NextGridRowToPlay], SettingsMap);
                    //StatusBar1->SetStatusText(msg);
                    //wxString effect=SettingsMap["E1_Effect"]+","+SettingsMap["E2_Effect"];
                    //TextCtrlLog->AppendText(msg+" "+effect+"\n");
                    xLights->UpdateBufferPaletteFromMap(1,SettingsMap,buffer);
                    xLights->UpdateBufferPaletteFromMap(2,SettingsMap,buffer);
                    buffer.SetMixType(SettingsMap["LayerMethod"]);
                    xLights->ResetEffectStates(ResetEffectState);
                    int freq=wxAtoi(SettingsMap["ID_SLIDER_SparkleFrequency"]);
                    if (freq == xLights->Slider_SparkleFrequency->GetMax()) freq=0;
                    buffer.SetSparkle(freq);

                    int brightness=wxAtoi(DefaultAs(SettingsMap["ID_SLIDER_Brightness"], wxString("100"))); //set to a safe value if missing -DJ
                    buffer.SetBrightness(brightness);
                    //                    int b = ModelBrightness;

                    int contrast=wxAtoi(SettingsMap["ID_SLIDER_Contrast"]);
                    buffer.SetContrast(contrast);
                    xLights->UpdateBufferFadesFromMap(1, SettingsMap,buffer);
                    xLights->UpdateBufferFadesFromMap(2, SettingsMap,buffer);
                    xLights->UpdateFitToTimeFromMap(1, SettingsMap,buffer);
                    xLights->UpdateFitToTimeFromMap(2, SettingsMap,buffer);

                    int effectMixThreshold=wxAtoi(SettingsMap["ID_SLIDER_EffectLayerMix"]);
                    buffer.SetMixThreshold(effectMixThreshold, wxAtoi(SettingsMap["ID_CHECKBOX_LayerMorph"]) != 0); //allow threshold to vary -DJ
                    //                    debug(1, "render seq data[%d]: set mix thresh %d, varies? %d", NextGridRowToPlay, effectMixThreshold, CheckBox_LayerMorph->GetValue());
                }

                int calcedNextRow = xLights->UpdateEffectDuration(!EffectStr.IsEmpty(),NextGridRowToPlay, buffer, myCol);

                crit.Enter();
                prev = prevCompleted;
                crit.Leave();
                while (calcedNextRow > prevCompleted)
                {
                    //spin until we can continue, should replace with a semaphore or something
                    wxMilliSleep(2);
                    crit.Enter();
                    prev = prevCompleted;
                    crit.Leave();
                }
                NextGridRowToPlay = calcedNextRow;
            } //  if (NextGridRowToPlay < rowcnt && msec >= GetGridStartTimeMSec(NextGridRowToPlay))
            bool effectsToUpdate = xLights->RenderEffectFromMap(0, p, SettingsMap,buffer, ResetEffectState, !onMainThread);
            effectsToUpdate |= xLights->RenderEffectFromMap(1, p, SettingsMap,buffer, ResetEffectState, !onMainThread);

            if (effectsToUpdate)
            {
                bufferClear = false;
                //TextCtrlLog->AppendText(wxString::Format("  period %d\n",p));
                buffer.CalcOutput(p);
                // update SeqData with contents of buffer
                size_t chnum;
                wxByte intensity;
                size_t NodeCnt=buffer.GetNodeCount();
                size_t cn=buffer.ChannelsPerNode();
                for(size_t n=0; n<NodeCnt; n++)
                {
                    for(size_t c=0; c<cn; c++)
                    {
                        buffer.GetChanIntensity(n,c,&chnum,&intensity);
                        xLights->SeqData[chnum*xLights->SeqNumPeriods+p]=intensity;
                    }
                }
            }//if (effectsToUpdate)
        } //for (int p=0; p<SeqNumPeriods; p++)

        wxString msg=_(wxString::Format("Finished saving %s",ColName));
        wxCriticalSection crit;
        crit.Enter();
        completed = effects.size() + 100;
        thread1Mutex.Lock();
        if (threads[myCol + 1] != NULL)
        {
            threads[myCol + 1]->SetPreviousColCompleted(completed);
        }
        threads[myCol] = NULL;
        renderMessages.push_back(msg);
        thread1Condition.Broadcast();
        thread1Mutex.Unlock();
        thread1Condition.Signal();
        crit.Leave();
        return NULL;
    }

    PixelBufferClass &GetBuffer()
    {
        return buffer;
    }
    int GetRowCompleted()
    {
        return completed;
    }
    void SetOnMainThread()
    {
        onMainThread = true;
    }
private:
    int firstRow;
    int seqNumPeriods;
    int myCol;
    wxString ColName;
    volatile int prevCompleted;
    int completed;
    bool onMainThread = false;

    wxXmlNode *ModelNode;

    PixelBufferClass buffer;
    bool ResetEffectState[2];
    std::vector<wxString> effects;
    std::vector<long> startTimes;
    xLightsFrame *xLights;
    xLightsRenderThread ** threads;
};



void xLightsFrame::RenderGridToSeqData()
{
    wxString ColName,msg, EffectStr;
    size_t ChannelLimit, NodeCnt;
    int rowcnt=Grid1->GetNumberRows();
    int colcnt=Grid1->GetNumberCols();
    wxXmlNode *ModelNode;

    xLightsRenderThread ** threads = new xLightsRenderThread*[colcnt + 1];
    for (int x = 0 ; x < colcnt + 1; x++)
    {
        threads[x] = NULL;
    }
    int firstColToRender = XLIGHTS_SEQ_STATIC_COLUMNS;
    int firstRowToRender = 0;
    if (FastSave_CheckBox->IsChecked())
    {
//TODO: track first AND last row + column changed since latest render
        firstColToRender = changedColumn;
        firstRowToRender = changedRow;
        if (firstColToRender < XLIGHTS_SEQ_STATIC_COLUMNS)
        {
            firstColToRender = XLIGHTS_SEQ_STATIC_COLUMNS;
        }
    }

    for (int c=firstColToRender; c<colcnt; c++) //c iterates through the columns of Grid1 retriving the effects for each model in the sequence.
    {
        threads[c] = NULL;
        ColName=Grid1->GetColLabelValue(c);
        ModelNode=GetModelNode(ColName);
        if (!ModelNode)
        {
            continue;
        }

        xLightsRenderThread *thread = new xLightsRenderThread(c, ColName, ModelNode, SeqNumPeriods, this, threads);
        threads[c] = thread;
        if (c == firstColToRender)
        {
            thread->SetPreviousColCompleted(rowcnt);
        }
        thread->setFirstRowToRender(firstRowToRender);

        NodeCnt=thread->GetBuffer().GetNodeCount();
        ChannelLimit=thread->GetBuffer().GetLastChannel() + 1;


        if (ChannelLimit > SeqNumChannels)
        {
            // need to add more channels to existing sequence
            msg=wxString::Format("Increasing sequence channel count from %ld to %d",SeqNumChannels,ChannelLimit);
            if (ChannelLimit > NetInfo.GetTotChannels())
            {
                msg+="\n\nEither your model is incorrect or the networks you have defined on the Setup Tab are incorrect.\n\nYou should fix this before doing any more exports!";
            }
            wxMessageBox(msg);
            SeqNumChannels=ChannelLimit;
            SeqDataLen=SeqNumChannels*SeqNumPeriods;
            SeqData.resize(SeqDataLen,0);
        }
        for (int x = 0; x < rowcnt; x++)
        {
            thread->AddEffectString(GetGridStartTimeMSec(x), Grid1->GetCellValue(x,c));
        }
        if (!threadedSave)
        {
            thread->SetOnMainThread();
            thread->SetPreviousColCompleted(rowcnt);
            thread->Entry();
            delete thread;
            threads[c] = NULL;
        }
    }
    if (threadedSave)
    {
        for (int x = 0; x < colcnt; x++)
        {
            if (threads[x] != NULL) {
                if (threads[x]->Run() != wxTHREAD_NO_ERROR )
                {
                    wxMessageBox("Could not create a render thread");
                    delete threads[x];
                    threads[x] = NULL;
                }
            }
        }

        thread1Mutex.Lock();
        for (int x = 0; x < colcnt; x++)
        {
            while (threads[x] != NULL)
            {
                for (int y = 0; y < effectsToRender.size(); y++)
                {
                    EffectEvent *ev = effectsToRender[y];
                    ev->mutex.Lock();
                    ev->returnVal = RenderEffectFromMap(ev->layer, ev->period,
                                                        *ev->SettingsMap,
                                                        *ev->buffer, ev->ResetEffectState);
                    ev->done = true;
                    ev->mutex.Unlock();
                }
                effectsToRender.clear();

                for (int y = 0; y < renderMessages.size(); y++)
                {
                    SetStatusText(renderMessages[y]);
                }
                renderMessages.clear();
                thread1Mutex.Unlock();
                wxYield(); //yield now to get the status out
                wxMilliSleep(1);
                thread1Mutex.Lock();
            }
        }
        thread1Mutex.Unlock();
    }

    thread1Mutex.Lock();
    for (int y = 0; y < renderMessages.size(); y++)
    {
        SetStatusText(renderMessages[y]);
    }
    renderMessages.clear();
    thread1Mutex.Unlock();
    delete [] threads;
    changedColumn = 99999;
    changedRow = 99999;
}

//FR Caller is responsible for deleting the returned data object
SeqDataType* xLightsFrame::RenderModelToData(wxXmlNode *modelNode, PixelBufferClass &buffer)
{
    MapStringString SettingsMap;
    wxString ColName,msg, EffectStr;
    long msec;
    bool effectsToUpdate;
    size_t  NodeCnt;
    int rowcnt=Grid1->GetNumberRows();
    int colcnt=Grid1->GetNumberCols();
    SeqDataType* retData;



    //buffer.InitBuffer(modelNode);
    NodeCnt = buffer.GetNodeCount();
    int max = buffer.GetChanCount() * SeqNumPeriods;
    retData = new SeqDataType(max);

    size_t firstChannel = 0;
    if (modelNode->GetAttribute("exportFirstStrand") != "")
    {
        int firstNode = 0;
        firstNode = wxAtoi(modelNode->GetAttribute("exportFirstStrand"));
        AppendConvertStatus(wxString::Format("First from model: %d\n", firstNode));
        if (firstNode < 1)
        {
            firstNode = 1;
        }
        firstNode--;
        int PixelsPerStrand=wxAtoi(modelNode->GetAttribute("parm2")) / wxAtoi(modelNode->GetAttribute("parm3"));
        AppendConvertStatus(wxString::Format("Pixels per strand: %d\n", PixelsPerStrand));
        firstNode *= PixelsPerStrand;
        AppendConvertStatus(wxString::Format("Calced firstNode: %d\n", firstNode));
        if (firstNode > NodeCnt)
        {
            firstNode = 0;
        }
        if (firstNode > 0)
        {
            size_t chnum;
            unsigned char intensity;
            buffer.GetChanIntensity(firstNode, 0, &firstChannel, &intensity);
            AppendConvertStatus(wxString::Format("firstNode: %d     NodeCnt:  %d    FirstChannel: %d\n",
                                                 firstNode, NodeCnt, firstChannel));
        }
    }


    bool ResetEffectState[2];
    ResetEffectStates(ResetEffectState);
    LoadSettingsMap("None,None,Effect 1", SettingsMap);
    for (int c=XLIGHTS_SEQ_STATIC_COLUMNS; c<colcnt; c++) //c iterates through the columns of Grid1 retriving the effects for each model in the sequence.
    {
        ColName=Grid1->GetColLabelValue(c);
        if (ColName != buffer.name) continue;

        int NextGridRowToPlay=0;
        for (int p=0; p<SeqNumPeriods; p++)
        {
            msec=p * XTIMER_INTERVAL;

            wxString effect1=SettingsMap["E1_Effect"];
            wxString effect2=SettingsMap["E1_Effect"];
            int persist1=wxAtoi(SettingsMap["E1_CHECKBOX_OverlayBkg"]);
            int persist2=wxAtoi(SettingsMap["E2_CHECKBOX_OverlayBkg"]);

            if (!persist1 || "None" == effect1)
            {
                buffer.Clear(0); //allow effects to overlay onto other effects (useful for composite models) -DJ
            }
            if (!persist2 || "None" == effect2)
            {
                buffer.Clear(1); //allow effects to overlay onto other effects (useful for composite models) -DJ
            }


            if (NextGridRowToPlay < rowcnt && msec >= GetGridStartTimeMSec(NextGridRowToPlay))
            {
                // start next effect
                wxYield();
                StatusBar1->SetStatusText(_(wxString::Format("%s: Saving row %ld/%ld",ColName,NextGridRowToPlay+1, Grid1->GetRows())));

                EffectStr=Grid1->GetCellValue(NextGridRowToPlay,c);
                EffectStr.Trim();
                if (!EffectStr.IsEmpty())
                {
                    //If the new cell is empty we will let the state variable keep ticking so that effects do not jump
                    LoadSettingsMap(Grid1->GetCellValue(NextGridRowToPlay,c), SettingsMap);
                    // TextCtrlLog->AppendText("effect"+LayerStr+"="+effect+", speed="+SpeedStr+"\n");
                    UpdateBufferPaletteFromMap(1,SettingsMap, buffer);
                    UpdateBufferPaletteFromMap(2,SettingsMap, buffer);
                    buffer.SetMixType(SettingsMap["LayerMethod"]);
                    ResetEffectStates(ResetEffectState);
                    int freq=wxAtoi(SettingsMap["ID_SLIDER_SparkleFrequency"]);
                    if (freq == Slider_SparkleFrequency->GetMax()) freq=0;
                    buffer.SetSparkle(freq);

                    int brightness=wxAtoi(SettingsMap["ID_SLIDER_Brightness"]);
                    buffer.SetBrightness(brightness);

                    int contrast=wxAtoi(SettingsMap["ID_SLIDER_Contrast"]);
                    buffer.SetContrast(contrast);
                    UpdateBufferFadesFromMap(1, SettingsMap, buffer);
                    UpdateBufferFadesFromMap(2, SettingsMap, buffer);
                    UpdateFitToTimeFromMap(1, SettingsMap, buffer);
                    UpdateFitToTimeFromMap(2, SettingsMap, buffer);
                }

                UpdateEffectDuration(!EffectStr.IsEmpty(), NextGridRowToPlay, buffer, c);
                NextGridRowToPlay++;
            } //  if (NextGridRowToPlay < rowcnt && msec >= GetGridStartTimeMSec(NextGridRowToPlay))
            effectsToUpdate = RenderEffectFromMap(0, p, SettingsMap, buffer, ResetEffectState);
            effectsToUpdate |= RenderEffectFromMap(1, p, SettingsMap, buffer, ResetEffectState);

            if (effectsToUpdate)
            {
                buffer.CalcOutput(p);
                // update SeqData with contents of buffer
                size_t chnum;
                wxByte intensity;
                size_t NodeCnt=buffer.GetNodeCount();
                size_t cn=buffer.ChannelsPerNode();
                for(size_t n=0; n<NodeCnt; n++)
                {
                    for(size_t c=0; c<cn; c++)
                    {
                        buffer.GetChanIntensity(n,c,&chnum,&intensity);
                        if (chnum >= firstChannel)
                        {
                            chnum -= firstChannel;
                        }
                        else
                        {
                            chnum = cn * NodeCnt - firstChannel + chnum;
                        }
                        unsigned long idx = chnum * SeqNumPeriods + p;
                        (*retData)[idx]=intensity;
                    }
                }
            }//if (effectsToUpdate)
        } //for (int p=0; p<SeqNumPeriods; p++)
    }
    return retData;
}
void xLightsFrame::OnBitmapButtonSaveSeqClick(wxCommandEvent& event)
{
    SaveSequence();
}


void xLightsFrame::SaveSequence()
{
    wxString NewFilename;
    bool ok;
    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }

    // save Grid1 to xml
    int rowcnt=Grid1->GetNumberRows();
    int colcnt=Grid1->GetNumberCols();
    if (colcnt <= XLIGHTS_SEQ_STATIC_COLUMNS)
    {
        wxMessageBox("No models in the grid!", "Warning");
    }
    if (rowcnt == 0)
    {
        wxMessageBox("No grid rows to save!", "Warning");
    }
    if (xlightsFilename.IsEmpty())
    {
        wxTextEntryDialog dialog(this,"Enter a name for the sequence:","Save As");
        do
        {
            if (dialog.ShowModal() != wxID_OK)
            {
                return;
            }
            // validate inputs
            NewFilename=dialog.GetValue();
            NewFilename.Trim();
            ok=true;
            if (NewFilename.IsEmpty())
            {
                ok=false;
                wxMessageBox(_("File name cannot be empty"), _("ERROR"));
            }
        }
        while (!ok);
        wxFileName oName(NewFilename);
        oName.SetPath( CurrentDir );
        oName.SetExt(_(XLIGHTS_SEQUENCE_EXT));
        DisplayXlightsFilename(oName.GetFullPath());

        oName.SetExt("xml");
        SeqXmlFileName=oName.GetFullPath();
    }

    EnableSequenceControls(false);

    wxStopWatch sw; // start a stopwatch timer



    StatusBar1->SetStatusText(_("Saving ")+xlightsFilename);
    int r,c;
    wxXmlDocument doc;
    wxXmlNode *tr, *td;
    wxXmlNode* root = new wxXmlNode( wxXML_ELEMENT_NODE, "xsequence" );
    doc.SetRoot( root );
    root->AddAttribute("BaseChannel", wxString::Format("%ld",SeqBaseChannel));
    root->AddAttribute("ChanCtrlBasic", SeqChanCtrlBasic ? "1" : "0");
    root->AddAttribute("ChanCtrlColor", SeqChanCtrlColor ? "1" : "0");

    // new items get added to the TOP of the xml structure, so add everything in reverse order

    // save data rows
    for (r=rowcnt-1; r>=0; r--)
    {
        tr=new wxXmlNode(root, wxXML_ELEMENT_NODE, "tr");
        for (c=colcnt-1; c>=0; c--)
        {
            td=new wxXmlNode(tr, wxXML_ELEMENT_NODE, "td");
            td->AddChild(new wxXmlNode(td, wxXML_TEXT_NODE, wxEmptyString, Grid1->GetCellValue(r,c)));
            td->AddAttribute("Protected",(Grid1->GetCellTextColour(r,c) == *wxBLUE)?"1":"0");
        }
    }

    // save labels to first row
    tr=new wxXmlNode(root, wxXML_ELEMENT_NODE, "tr");
    for (c=colcnt-1; c>=0; c--)
    {
        td=new wxXmlNode(tr, wxXML_ELEMENT_NODE, "td");
        td->AddChild(new wxXmlNode(td, wxXML_TEXT_NODE, wxEmptyString, Grid1->GetColLabelValue(c)));
    }
    doc.Save(SeqXmlFileName);

    RenderGridToSeqData();  // incorporate effects into xseq file
    WriteXLightsFile(xlightsFilename);
    UnsavedChanges = false;
    float elapsedTime = sw.Time()/1000.0; // now stop stopwatch timer and get elapsed time. change into seconds from ms
    wxString displayBuff = wxString::Format(_("%s     Updated in %7.3f seconds"),xlightsFilename,elapsedTime);
    StatusBar1->SetStatusText(displayBuff);
    //  StatusBar1->SetStatusText(_("Updated ")+xlightsFilename);

    EnableSequenceControls(true);
}

void xLightsFrame::LoadSettingsMap(wxString settings, MapStringString& SettingsMap)
{
    wxString before,after,name,value;
    int cnt=0;
    while (!settings.IsEmpty())
    {
        before=settings.BeforeFirst(',');
        after=settings.AfterFirst(',');
        switch (cnt)
        {
        case 0:
            SettingsMap.clear();
            SettingsMap["E1_Effect"]=before;
            break;
        case 1:
            SettingsMap["E2_Effect"]=before;
            break;
        case 2:
            SettingsMap["LayerMethod"]=before;
            break;
        default:
            name=before.BeforeFirst('=');
            value=before.AfterFirst('=');
            SettingsMap[name]=value;
            break;
        }
        settings=after;
        cnt++;
    }
}

void xLightsFrame::OnBitmapButtonInsertRowClick(wxCommandEvent& event)
{
    InsertRow();
}

void xLightsFrame::InsertRow()
{
    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }
    int r=Grid1->GetGridCursorRow();
    GridCellChanged(r + 1, 0);
    Grid1->InsertRows(r + 1, 1);
    // only the first 2 columns are editable; set everything else to read-only
    int n=Grid1->GetNumberCols();
    for (int c=XLIGHTS_SEQ_STATIC_COLUMNS; c < n; c++)
    {
        Grid1->SetReadOnly(r,c);
    }
    UnsavedChanges = true;
}

void xLightsFrame::OnBitmapButtonDeleteRowClick(wxCommandEvent& event)
{
    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }
    if ( Grid1->IsSelection() )
    {
        wxGridUpdateLocker locker(Grid1);
        int c=Grid1->GetGridCursorCol();
        for (int n = Grid1->GetNumberRows() - 1; n >= 0; n--)
        {
            if ( Grid1->IsInSelection( n , c ) )
            {
                Grid1->DeleteRows( n, 1 );
                GridCellChanged(n, 0);
                UnsavedChanges = true;
            }
        }
    }
    else
    {
        int r=Grid1->GetGridCursorRow();
        Grid1->DeleteRows( r, 1 );
        GridCellChanged(r, 0);
        UnsavedChanges = true;
    }
}

void xLightsFrame::OnButtonDisplayElementsClick(wxCommandEvent& event)
{
    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }
    ChooseModelsForSequence();
}

//copy row2 to row1
void xLightsFrame::CopyRow(int row1, int row2)
{
    int i, iMax;
    iMax = Grid1->GetNumberCols();
    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    for(i=0; i<iMax; i++)
    {
        Grid1->SetCellValue(row1,i,Grid1->GetCellValue(row2,i));
    }
    GridCellChanged(row1, 0);
    GridCellChanged(row2, 0);
}

// returns time in milliseconds
long xLightsFrame::GetGridStartTimeMSec(int row)
{
    double d=GetGridStartTime(row);
    return long(d*1000.0);
}

// returns time in seconds
double xLightsFrame::GetGridStartTime(int row)
{
    double d;
    wxString s = Grid1->GetCellValue(row,0);
    if (!s.IsEmpty() && s.ToDouble(&d)) return d;
    return 999999.9;
}

// see http://en.wikipedia.org/wiki/Insertion_sort
void xLightsFrame::NumericSort()
{
    //sort the table
    int i, iHole;
    double d;

    Grid1->BeginBatch();
    int rowcnt = Grid1->GetNumberRows();
    Grid1->AppendRows(1); // temp swap row

    for (i=1; i < rowcnt; i++)
    {
        d=GetGridStartTime(i);
        CopyRow(rowcnt,i);
        iHole = i;
        while ((iHole > 0) && (GetGridStartTime(iHole-1) > d))
        {
            CopyRow(iHole,iHole-1);
            iHole--;
        }
        CopyRow(iHole,rowcnt);
    }
    Grid1->DeleteRows(rowcnt,1);
    Grid1->EndBatch();
    Grid1->ForceRefresh();
    GridCellChanged(0, 0);
}

void xLightsFrame::OnGrid1CellChange(wxGridEvent& event)
{
    int col = event.GetCol();
    if (col==0)
    {
        // re-order table by start time (column 0)
        Grid1->EnableEditing(false);
        //StatusBar1->SetStatusText(_("New grid value=")+Grid1->GetCellValue(row,col));
        NumericSort();
        Grid1->EnableEditing(true);
    }
    UnsavedChanges = true;
    event.Skip();
}

void xLightsFrame::OnGrid1CellLeftClick(wxGridEvent& event)
{
    int row = event.GetRow(),
        col = event.GetCol();

    if ( row != effGridPrevY || col != effGridPrevX)
    {
        //set selected cell background
        Grid1->SetCellBackgroundColour( *wxYELLOW, row, col);
        Grid1->SetCellBackgroundColour(Grid1->GetDefaultCellBackgroundColour(), effGridPrevY, effGridPrevX);
    }
    effGridPrevY = row;
    effGridPrevX = col;

    if (col >= XLIGHTS_SEQ_STATIC_COLUMNS)
    {
        wxString EffectString=Grid1->GetCellValue(row,col);
        if (!EffectString.IsEmpty())
        {
            //Choice_Presets->SetSelection(0);  // set to <grid>
            SetEffectControls(EffectString, Grid1->GetColLabelValue(col));
        }
    }
    Grid1->ForceRefresh();
    event.Skip();
}

void xLightsFrame::ClearEffectWindow()
{
    //wxClientDC dc(ScrolledWindow1); //dp
    //dc.Clear();
}

void xLightsFrame::OnButtonSeqExportClick(wxCommandEvent& event)
{
    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }
    if (Grid1->GetNumberCols() <= XLIGHTS_SEQ_STATIC_COLUMNS)
    {
        wxMessageBox("No models in the grid!", "Error");
        return;
    }
    if (Grid1->GetNumberRows() == 0)
    {
        wxMessageBox("No grid rows to save!", "Error");
        return;
    }
    int DlgResult;
    bool ok;
    wxString filename;
    SeqExportDialog dialog(this);
    do
    {
        ok=true;
        DlgResult=dialog.ShowModal();
        if (DlgResult == wxID_OK)
        {
            // validate inputs
            filename=dialog.TextCtrlFilename->GetValue();
            filename.Trim();
            if (filename.IsEmpty())
            {
                ok=false;
                wxMessageBox(_("The file name cannot be empty"), _("ERROR"));
            }
        }
    }
    while (DlgResult == wxID_OK && !ok);
    if (DlgResult != wxID_OK) return;

    RenderGridToSeqData();


    wxString format=dialog.ChoiceFormat->GetStringSelection();
    wxStopWatch sw;
    wxString Out3=format.Left(3);
    StatusBar1->SetStatusText(_("Starting Export for ") + format + "-" + Out3);
    wxYield();

    if (Out3 == "LSP")
    {
        filename = filename + "_USER";
    }
    wxFileName oName(filename);
    oName.SetPath( CurrentDir );
    wxString fullpath;


    // REFACTOR -- FR: These extensions should all be based on Macros. For that matter all these compares should be as well.
    if (Out3 == "LOR")
    {
        if (mediaFilename.IsEmpty())
        {
            oName.SetExt(_("las"));
        }
        else
        {
            oName.SetExt(_("lms"));
        }
        fullpath=oName.GetFullPath();
        WriteLorFile(fullpath);
    }
    else if (Out3 == "Lcb")
    {
        oName.SetExt(_("lcb"));
        fullpath=oName.GetFullPath();
        WriteLcbFile(fullpath);
    }
    else if (Out3 == "Vix")
    {
        oName.SetExt(_("vix"));
        fullpath=oName.GetFullPath();
        WriteVixenFile(fullpath);
    }
    else if (Out3 == "Vir")
    {
        oName.SetExt(_("vir"));
        fullpath=oName.GetFullPath();
        WriteVirFile(fullpath);
    }
    else if (Out3 == "LSP")
    {
        oName.SetExt(_("xml"));
        fullpath=oName.GetFullPath();
        WriteLSPFile(fullpath);
        return;
    }
    else if (Out3 == "HLS")
    {
        oName.SetExt(_("hlsnc"));
        fullpath=oName.GetFullPath();
        WriteHLSFile(fullpath);
    }
    else if (Out3 == "xLi")
    {
        oName.SetExt(_(XLIGHTS_SEQUENCE_EXT));
        fullpath=oName.GetFullPath();
        WriteXLightsFile(fullpath);
    }
    else if (Out3 == "Fal")
    {
        oName.SetExt(_("fseq"));
        fullpath=oName.GetFullPath();
        WriteFalconPiFile(fullpath);
    }

    StatusBar1->SetStatusText(_("Finished writing: " )+fullpath + wxString::Format(" in %ld ms ",sw.Time()));
}

void xLightsFrame::OnbtEditViewsClick(wxCommandEvent& event)
{
    int DlgResult;
    ViewsDialog dialog(this);
    dialog.SetModelAndViewNodes(ModelsNode,ViewsNode);
    dialog.CenterOnParent();
    dialog.ShowModal();
    SaveEffectsFile();
    UpdateViewList();
}


wxXmlNode* xLightsFrame::SelectModelToExport()
{
    ExportModelSelect dialog(this);
    wxString modelName;
    int colcnt = Grid1->GetNumberCols();
    wxASSERT(colcnt > XLIGHTS_SEQ_STATIC_COLUMNS);
    for( int col=XLIGHTS_SEQ_STATIC_COLUMNS; col<colcnt; col++)
    {
        dialog.ModelChoice->Append(Grid1->GetColLabelValue(col));
    }
    dialog.ModelChoice->SetSelection(0);
    if (dialog.ShowModal() != wxID_OK) return NULL;
    return GetModelNode(dialog.ModelChoice->GetStringSelection());
}


void xLightsFrame::OnButtonModelExportClick(wxCommandEvent& event)
{
    if (SeqData.size() == 0)
    {
        wxMessageBox("You must open a sequence first!", "Error");
        return;
    }
    if (Grid1->GetNumberCols() <= XLIGHTS_SEQ_STATIC_COLUMNS)
    {
        wxMessageBox("No models in the grid!", "Error");
        return;
    }
    if (Grid1->GetNumberRows() == 0)
    {
        wxMessageBox("No grid rows to save!", "Error");
        return;
    }
    int DlgResult;
    bool ok;
    wxString filename;
    wxXmlNode* modelNode;
    SeqDataType* dataBuf;
    int numChan;
    SeqExportDialog dialog(this);
    dialog.ModelExportTypes();
    do
    {
        ok=true;
        DlgResult=dialog.ShowModal();
        if (DlgResult == wxID_OK)
        {
            // validate inputs
            filename=dialog.TextCtrlFilename->GetValue();
            filename.Trim();
            if (filename.IsEmpty())
            {
                ok=false;
                wxMessageBox(_("The file name cannot be empty"), _("ERROR"));
            }
        }
    }
    while (DlgResult == wxID_OK && !ok);
    if (DlgResult != wxID_OK) return;

    modelNode = SelectModelToExport();

    if(!modelNode) return;

    PixelBufferClass *buffer = new PixelBufferClass();
    buffer->InitBuffer(modelNode,true);
    numChan = buffer->GetChanCount();
    int cpn = buffer->GetChanCountPerNode();
    dataBuf = RenderModelToData(modelNode, *buffer);
    delete buffer;

    wxString format=dialog.ChoiceFormat->GetStringSelection();
    wxStopWatch sw;
    wxString Out3=format.Left(3);

    if (Out3 == "LSP")
    {
        filename = filename + "_USER";
    }
    wxFileName oName(filename);
    oName.SetPath( CurrentDir );
    wxString fullpath;

    StatusBar1->SetStatusText(_("Starting Export for ") + format + "-" + Out3);
    wxYield();

    if (Out3 == "Lcb")
    {
        oName.SetExt(_("lcb"));
        fullpath=oName.GetFullPath();
        WriteLcbFile(fullpath, numChan, SeqNumPeriods, dataBuf);
    }
    else if (Out3 == "Vir")
    {
        oName.SetExt(_("vir"));
        fullpath=oName.GetFullPath();
        WriteVirFile(fullpath, numChan, SeqNumPeriods, dataBuf);
    }
    else if (Out3 == "LSP")
    {
        oName.SetExt(_("xml"));
        fullpath=oName.GetFullPath();
        //    int cpn = ChannelsPerNode();
        WriteLSPFile(fullpath, numChan, SeqNumPeriods, dataBuf, cpn);
        return;
    }
    else if (Out3 == "HLS")
    {
        oName.SetExt(_("hlsnc"));
        fullpath=oName.GetFullPath();
        WriteHLSFile(fullpath, numChan, SeqNumPeriods, dataBuf);
    }
    else if (Out3 == "Fal")
    {
        wxString tempstr;
        long stChan;
        tempstr=modelNode->GetAttribute("StartChannel","1");
        tempstr.ToLong(&stChan);
        oName.SetExt(_("eseq"));
        fullpath=oName.GetFullPath();
        WriteFalconPiModelFile(fullpath, numChan, SeqNumPeriods, dataBuf, stChan, numChan);
    }

    delete dataBuf;
    StatusBar1->SetStatusText(_("Finished writing model: " )+fullpath + wxString::Format(" in %ld ms ",sw.Time()));
}

void xLightsFrame::OnGrid1CellRightClick(wxGridEvent& event)
{
    wxMenu mnu;
    curCell->Set(event.GetRow(), event.GetCol());

    mnu.Append(ID_PROTECT_EFFECT, 	"Protect Effect");
    mnu.Append(ID_UNPROTECT_EFFECT, "Unprotect Effect");
    mnu.AppendSeparator();
    mnu.Append(ID_RANDOM_EFFECT, 	"Create Random Effect");
    mnu.Append(ID_IGNORE_CLICK, 	"Ignore Click");
    mnu.AppendSeparator();
    mnu.Append(ID_DELETE_EFFECT, 	"Delete Highlighted Effect");
    mnu.Append(ID_COPYROW_EFFECT, 	"Copy Effect Across Row"); //requested by Sean -DJ
    mnu.Append(ID_CLEARROW_EFFECT, 	"Clear Row"); //requested by Sean -DJ
    mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&xLightsFrame::OnPopupClick, NULL, this);
    PopupMenu(&mnu);
}


static const long ID_SHIFT_COL_LEFT = wxNewId();
static const long ID_SHIFT_COL_RIGHT = wxNewId();

void xLightsFrame::OnGrid1LabelRightClick(wxGridEvent& event)
{
    if (event.GetCol() > 1)
    {
        wxMenu mnu;
        curCell->Set(event.GetRow(), event.GetCol());
        if (event.GetCol() > 2)
        {
            mnu.Append(ID_SHIFT_COL_LEFT, "Shift Left");
        }
        if (event.GetCol() != Grid1->GetNumberCols() - 1)
        {
            mnu.Append(ID_SHIFT_COL_RIGHT, "Shift Right");
        }
        Grid1->SelectCol(event.GetCol());;

        mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&xLightsFrame::OnPopupClick, NULL, this);
        PopupMenu(&mnu);
    }
}

void xLightsFrame::OnPopupClick(wxCommandEvent &event)
{
    if(event.GetId() == ID_DELETE_EFFECT)
    {
        DeleteSelectedEffects(event);
    }
    if(event.GetId() == ID_RANDOM_EFFECT)
    {
        InsertRandomEffects(event);
    }
    if(event.GetId() == ID_PROTECT_EFFECT)
    {
        ProtectSelectedEffects(event);
    }
    if(event.GetId() == ID_UNPROTECT_EFFECT)
    {
        UnprotectSelectedEffects(event);
    }
    if (event.GetId() == ID_COPYROW_EFFECT)
        CopyEffectAcrossRow(event); //-DJ
    if (event.GetId() == ID_CLEARROW_EFFECT)
        ClearEffectRow(event); //-DJ

    if (event.GetId() == ID_SHIFT_COL_LEFT)
    {
        SwapCols(curCell->GetCol() - 1, curCell->GetCol());
        Grid1->SelectCol(curCell->GetCol() - 1);
    }
    if (event.GetId() == ID_SHIFT_COL_RIGHT)
    {
        SwapCols(curCell->GetCol(), curCell->GetCol() + 1);
        Grid1->SelectCol(curCell->GetCol() + 1);
    }
}

void xLightsFrame::SwapCols(int col1, int col2)
{
    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    GridCellChanged(0, col1);
    GridCellChanged(0, col2);
    for (int x = 0; x < Grid1->GetNumberRows(); x++)
    {
        wxString tmp = Grid1->GetCellValue(x, col1);
        wxColor tmpColor = Grid1->GetCellTextColour(x, col1);
        Grid1->SetCellValue(x, col1, Grid1->GetCellValue(x, col2));
        Grid1->SetCellTextColour(x, col1, Grid1->GetCellTextColour(x, col2));
        Grid1->SetCellValue(x, col2, tmp);
        Grid1->SetCellTextColour(x, col2, tmpColor);
    }
    wxString tmp = Grid1->GetColLabelValue(col1);
    Grid1->SetColLabelValue(col1, Grid1->GetColLabelValue(col2));
    Grid1->SetColLabelValue(col2, tmp);
}

//void djdebug(const char* fmt, ...); //_DJ
void xLightsFrame::OnbtRandomEffectClick(wxCommandEvent& event)
{
    int r,c;
    wxString v;

    int nRows = Grid1->GetNumberRows();
    int nCols = Grid1->GetNumberCols();
    bool apply_horiz = wxGetKeyState(WXK_SHIFT); //use Shift state to apply horizontally -DJ

#if 0 //debug
    wxString buf;
    for (auto it = buttonState.begin(); it != buttonState.end(); ++it)
        buf += it->first + wxString::Format("=%d,", it->second);
    for (auto it = EffectsPanel1->buttonState.begin(); it != EffectsPanel1->buttonState.end(); ++it)
        buf += "FX1:" + it->first + wxString::Format("=%d,", it->second);
    for (auto it = EffectsPanel2->buttonState.begin(); it != EffectsPanel2->buttonState.end(); ++it)
        buf += "FX2:" + it->first + wxString::Format("=%d,", it->second);
//    djdebug("InsertRandomEffects: %s", (const char*)buf.c_str());
//    djdebug("GetRandomEffectString: %s rnd? %d", (const char*)Slider_Speed->GetName().c_str(), isRandom(Slider_Speed));
#endif

//    if (apply_horiz) wxMessageBox(_("Apply horiz."), _("DEBUG")); //-DJ
//    for (c=XLIGHTS_SEQ_STATIC_COLUMNS; c<nCols; c++)
    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    for (r=0; r<nRows; r++) //reversed order of loops -DJ
    {
        v.Empty(); //no random effect for this row yet -DJ
//        for (r=0; r<nRows; r++)
        for (c=XLIGHTS_SEQ_STATIC_COLUMNS; c<nCols; c++)
        {
            if( Grid1->GetCellTextColour(r,c) == *wxBLACK )
            {
                if (!apply_horiz || v.empty()) v = CreateEffectStringRandom();
                Grid1->SetCellValue(r,c,v);
                GridCellChanged(r,c);
            }
        }
    }
    UnsavedChanges = true;
}

void xLightsFrame::OnSlider_EffectLayerMixCmdScroll(wxScrollEvent& event)
{
    txtCtlEffectMix->SetValue(wxString::Format( "%d",Slider_EffectLayerMix->GetValue()));
}

void xLightsFrame::OnSlider_SparkleFrequencyCmdScroll(wxScrollEvent& event)
{
    txtCtrlSparkleFreq->SetValue(wxString::Format("%d",Slider_SparkleFrequency->GetValue()));
}



void xLightsFrame::OnSlider_BrightnessCmdScroll(wxScrollEvent& event)
{
    txtCtlBrightness->SetValue(wxString::Format("%d",Slider_Brightness->GetValue()));
}

void xLightsFrame::OnSlider_ContrastCmdScroll(wxScrollEvent& event)
{
    txtCtlContrast->SetValue(wxString::Format("%d",Slider_Contrast->GetValue()));
}

void xLightsFrame::OnScrolledWindow1Resize(wxSizeEvent& event)
{
    //ScrolledWindow1->ClearBackground();
}

// pass true for cutting, false for copying
void xLightsFrame::CutOrCopyToClipboard(bool IsCut)
{
    int i,k;
    wxString copy_data;
    bool something_in_this_line;

    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    if (Grid1->IsSelection())
    {
        // some cells are selected
        for (i=0; i< Grid1->GetRows(); i++)        // step through all lines
        {
            something_in_this_line = false;             // nothing found yet
            for (k=0; k<Grid1->GetCols(); k++)     // step through all colums
            {
                if (Grid1->IsInSelection(i,k))     // this field is selected!!!
                {
                    if (!something_in_this_line)        // first field in this line => may need a linefeed
                    {
                        if (!copy_data.IsEmpty())       // ... if it is not the very first field
                        {
                            copy_data += "\n";     // next LINE
                        }
                        something_in_this_line = true;
                    }
                    else                                    // if not the first field in this line we need a field seperator (TAB)
                    {
                        copy_data += "\t";  // next COLUMN
                    }
                    copy_data += Grid1->GetCellValue(i,k);    // finally we need the field value
                    if (IsCut)
                    {
                        Grid1->SetCellValue(i,k,wxEmptyString);
                        GridCellChanged(i,k);
                        UnsavedChanges=true;
                    }
                }
            }
        }
    }
    else
    {
        // no cells selected, so copy current cell
        i = Grid1->GetGridCursorRow();
        k = Grid1->GetGridCursorCol();
        copy_data = Grid1->GetCellValue(i,k);
        if (IsCut)
        {
            Grid1->SetCellValue(i,k,wxEmptyString);
            GridCellChanged(i,k);
            UnsavedChanges=true;
        }
    }

    if (wxTheClipboard->Open())
    {
        if (!wxTheClipboard->SetData(new wxTextDataObject(copy_data)))
        {
            wxMessageBox(_("Unable to copy data to clipboard."), _("Error"));
        }
        wxTheClipboard->Close();
    }
    else
    {
        wxMessageBox(_("Error opening clipboard."), _("Error"));
    }
}

void xLightsFrame::OnBitmapButtonGridCutClick(wxCommandEvent& event)
{
    CutOrCopyToClipboard(true);
}

void xLightsFrame::OnBitmapButtonGridCopyClick(wxCommandEvent& event)
{
    CutOrCopyToClipboard(false);
}

// validate that s contains a valid effect
// just a placeholder for now
bool xLightsFrame::IsValidEffectString(wxString& s)
{
    return true;
}

void xLightsFrame::OnBitmapButtonGridPasteClick(wxCommandEvent& event)
{
    PasteFromClipboard();
}

void xLightsFrame::PasteFromClipboard()
{
    wxString copy_data;
    wxString cur_line;
    wxArrayString fields;
    int i,k,fieldnum;

    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported(wxDF_TEXT))
        {
            wxTextDataObject data;

            if (wxTheClipboard->GetData(data))
            {
                copy_data = data.GetText();
            }
            else
            {
                wxMessageBox(_("Unable to copy data from clipboard."), _("Error"));
            }
        }
        else
        {
            wxMessageBox(_("Non-Text data in clipboard."), _("Error"));
        }
        wxTheClipboard->Close();
    }
    else
    {
        wxMessageBox(_("Error opening clipboard."), _("Error"));
        return;
    }

    i = Grid1->GetGridCursorRow();
    k = Grid1->GetGridCursorCol();
    int numrows=Grid1->GetNumberRows();
    int numcols=Grid1->GetNumberCols();
    bool errflag=false;
#ifdef __WXOSX__
    copy_data.Replace("\r", "\n");
#endif
    WANT_FASTER_GRID(Grid1); //defer repaints until finished updating -DJ
    do
    {
        cur_line = copy_data.BeforeFirst('\n');
        copy_data = copy_data.AfterFirst('\n');
        fields=wxSplit(cur_line,'\t');
        for(fieldnum=0; fieldnum<fields.Count(); fieldnum++)
        {
            if (i < numrows && k+fieldnum < numcols /*&& k+fieldnum >=XLIGHTS_SEQ_STATIC_COLUMNS*/)
            {
                if (fields[fieldnum].IsEmpty() || IsValidEffectString(fields[fieldnum]))
                {
                    Grid1->SetCellValue(i,k+fieldnum,fields[fieldnum]);
                    GridCellChanged(i,k+fieldnum);
                    UnsavedChanges=true;
                }
                else
                {
                    errflag=true;
                }
            }
        }
        i++;
    }
    while (copy_data.IsEmpty() == false);
    if (errflag)
    {
        wxMessageBox(_("One or more of the values were not pasted because they did not contain a number"),_("Paste Error"));
    }
}

void xLightsFrame::AllRowsAreUpdated()
{
//    for(int row=0;row<Grid1->GetRows();row++)
//    {
//    }
}
