#ifndef EFFECTSPANEL_H
#define EFFECTSPANEL_H

//(*Headers(EffectsPanel)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/colordlg.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/choicebk.h>
//*)

#include <wx/filedlg.h>
#include <wx/fontdlg.h>

class EffectsPanel: public wxPanel
{
	public:

		EffectsPanel(wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name=wxPanelNameStr);
		virtual ~EffectsPanel();
        bool PaletteChanged;
        bool EffectChanged;
        void SetDefaultPalette();
        wxString* CurrentDir;
        wxColour GetPaletteColor(int idx);
        void SetPaletteColor(int idx, const wxColour* c);
        wxString GetEffectString();
        wxString GetRandomEffectString(int effidx);
        static const int PALETTE_SIZE = 6;

		//(*Declarations(EffectsPanel)
		wxPanel* Panel1_Life;
		wxButton* Button_Text_Font2;
		wxStaticText* StaticText93;
		wxSlider* Slider_Fireworks_Number_Explosions;
		wxStaticText* StaticText79;
		wxCheckBox* CheckBox_Meteors_FallUp;
		wxTextCtrl* TextCtrl_Fadein;
		wxSlider* Slider_Speed;
		wxStaticText* StaticText100;
		wxStaticText* StaticText32;
		wxStaticText* StaticText36;
		wxStaticText* StaticText40;
		wxCheckBox* CheckBox_Bars_Highlight;
		wxStaticText* StaticText27;
		wxStaticText* StaticText89;
		wxPanel* Panel1_Circles;
		wxSlider* Slider_Butterfly_Style;
		wxPanel* Panel1_Garlands;
		wxChoice* Choice_Meteors_Effect;
		wxSlider* Slider_Life_Count;
		wxSlider* Slider_Twinkle_Steps;
		wxStaticText* StaticText41;
		wxPanel* Panel1_Spirograph;
		wxButton* Button_Palette1;
		wxStaticText* StaticText85;
		wxSlider* Slider_Fireworks_Velocity;
		wxButton* Button_Pictures_Filename;
		wxStaticText* StaticText128;
		wxStaticText* StaticText46;
		wxCheckBox* CheckBox_Circles_Collide;
		wxSlider* Slider_Butterfly_Skip;
		wxCheckBox* CheckBox_Circles_Radial_3D;
		wxStaticText* StaticText92;
		wxStaticText* StaticText5;
		wxPanel* Panel_Text1;
		wxStaticText* StaticText87;
		wxStaticText* StaticText25;
		wxStaticText* StaticText108;
		wxPanel* Panel1_ColorWash;
		wxChoice* Choice_Text_Dir2;
		wxCheckBox* CheckBox_Palette3;
		wxChoice* Choice_Butterfly_Colors;
		wxSlider* Slider_Garlands_Type;
		wxChoice* Choice_Text_Dir1;
		wxStaticText* StaticText39;
		wxCheckBox* CheckBox_FitToTime;
		wxChoicebook* Choicebook1;
		wxStaticText* StaticText35;
		wxSlider* Slider_Meteors_Length;
		wxSlider* Slider_Fireworks_Fade;
		wxStaticText* StaticText78;
		wxStaticText* StaticText23;
		wxStaticText* StaticText33;
		wxCheckBox* CheckBox_Palette2;
		wxPanel* Panel1_Pictures;
		wxChoice* Choice_Text_Effect1;
		wxSlider* Slider_Fire_HueShift;
		wxPanel* Panel1_Meteors;
		wxSlider* Slider_Life_Seed;
		wxStaticText* StaticText90;
		wxSlider* Slider_Twinkle_Count;
		wxSlider* Slider_Fire_Height;
		wxSlider* Slider_Snowflakes_Type;
		wxButton* Button_Palette3;
		wxStaticText* StaticText130;
		wxPanel* Panel1_Piano;
		wxSlider* Slider_Meteors_Count;
		wxTextCtrl* TextCtrl_Text_Line2;
		wxChoice* Choice_Pictures_Direction;
		wxChoice* Choice_Meteors_Type;
		wxSlider* Slider_Spirals_Count;
		wxTextCtrl* TextCtrl_Speed;
		wxSlider* Slider_Bars_BarCount;
		wxCheckBox* CheckBox_Spirals_Blend;
		wxButton* Button_Palette6;
		wxStaticText* StaticText107;
		wxSlider* Slider_Spirograph_d;
		wxChoice* Choice_Bars_Direction;
		wxStaticText* StaticText57;
		wxCheckBox* CheckBox_Palette5;
		wxSlider* Slider_Spirals_Rotation;
		wxStaticText* StaticText98;
		wxButton* Button_Text_Font1;
		wxStaticText* StaticText43;
		wxStaticText* StaticText24;
		wxSlider* Slider_Spirograph_r;
		wxSlider* Slider_Pictures_GifSpeed;
		wxStaticText* StaticText94;
		wxSlider* Slider_Snowflakes_Count;
		wxTextCtrl* TextCtrl_Text_Font1;
		wxStaticText* StaticText88;
		wxStaticText* StaticText34;
		wxSlider* Slider_Tree_Branches;
		wxTextCtrl* TextCtrl_Text_Line1;
		wxStaticText* StaticText86;
		wxButton* Button_Palette4;
		wxStaticText* StaticText105;
		wxStaticText* StaticText37;
		wxCheckBox* CheckBox_Palette1;
		wxStaticText* StaticText51;
		wxStaticText* StaticText53;
		wxStaticText* StaticText132;
		wxCheckBox* CheckBox_Circles_Random_m;
		wxCheckBox* CheckBox_Palette6;
		wxStaticText* StaticText54;
		wxSlider* Slider_Meteors_Swirl_Intensity;
		wxPanel* Panel_Text2;
		wxPanel* Panel1_Snowflakes;
		wxCheckBox* CheckBox_Palette4;
		wxFlexGridSizer* FlexGridSizer_Palette;
		wxPanel* Panel1_Tree;
		wxStaticText* StaticText31;
		wxStaticText* StaticText103;
		wxPanel* Panel1_Fire;
		wxChoice* Choice_Text_Effect2;
		wxStaticText* StaticText96;
		wxPanel* Panel1_Twinkle;
		wxSlider* Slider_Spirals_Thickness;
		wxTextCtrl* TextCtrl_Text_Font2;
		wxStaticText* StaticText104;
		wxCheckBox* CheckBox_Twinkle_Strobe;
		wxStaticText* StaticText137;
		wxCheckBox* CheckBox_ColorWash_HFade;
		wxColourDialog* ColourDialog1;
		wxSlider* Slider_Garlands_Spacing;
		wxCheckBox* CheckBox_ColorWash_VFade;
		wxStaticText* StaticText136;
		wxPanel* Panel1_None;
		wxSlider* Slider_Text_Position1;
		wxStaticText* StaticText45;
		wxPanel* Panel1_Fireworks;
		wxChoice* Choice_Text_Count1;
		wxTextCtrl* TextCtrl_Pictures_Filename;
		wxSlider* Slider_Spirograph_R;
		wxStaticText* StaticText80;
		wxSlider* Slider_ColorWash_Count;
		wxButton* Button_Palette5;
		wxStaticText* StaticText59;
		wxStaticText* StaticText95;
		wxCheckBox* CheckBox_Circles_Radial;
		wxCheckBox* CheckBox_Fire_GrowFire;
		wxTextCtrl* TextCtrl_Fadeout;
		wxSlider* Slider_Circles_Count;
		wxStaticText* StaticText26;
		wxSlider* Slider_Snowstorm_Length;
		wxStaticText* StaticText28;
		wxStaticText* StaticText30;
		wxPanel* Panel1_Butterfly;
		wxPanel* Panel1_Spirals;
		wxSlider* Slider_Piano_Keyboard;
		wxSlider* Slider_Circles_Size;
		wxSlider* Slider_Snowstorm_Count;
		wxSlider* Slider_Fireworks_Count;
		wxCheckBox* CheckBox_Bars_3D;
		wxSlider* Slider_Butterfly_Chunks;
		wxStaticText* StaticText2;
		wxStaticText* StaticText1;
		wxSlider* Slider_Text_Position2;
		wxPanel* Panel1_Text;
		wxStaticText* StaticText83;
		wxStaticText* StaticText38;
		wxStaticText* StaticText91;
		wxStaticText* StaticText102;
		wxChoice* Choice_Text_Count2;
		wxNotebook* Notebook_Text1;
		wxCheckBox* CheckBox_Spirograph_Animate;
		wxCheckBox* CheckBox_Circles_Bounce;
		wxSlider* Slider_Spirals_Direction;
		wxCheckBox* CheckBox_Spirals_3D;
		wxButton* Button_Palette2;
		wxPanel* Panel1_Bars;
		wxPanel* Panel1_Snowstorm;
		wxStaticText* StaticText134;
		wxStaticText* StaticText3;
		//*)

	protected:

		//(*Identifiers(EffectsPanel)
		static const long ID_PANEL4;
		static const long ID_STATICTEXT25;
		static const long ID_SLIDER_Bars_BarCount;
		static const long ID_STATICTEXT26;
		static const long ID_CHOICE_Bars_Direction;
		static const long ID_CHECKBOX_Bars_Highlight;
		static const long ID_CHECKBOX_Bars_3D;
		static const long ID_PANEL8;
		static const long ID_STATICTEXT27;
		static const long ID_CHOICE_Butterfly_Colors;
		static const long ID_STATICTEXT28;
		static const long ID_SLIDER_Butterfly_Style;
		static const long ID_STATICTEXT30;
		static const long ID_SLIDER_Butterfly_Chunks;
		static const long ID_STATICTEXT31;
		static const long ID_SLIDER_Butterfly_Skip;
		static const long ID_PANEL10;
		static const long ID_STATICTEXT21;
		static const long ID_SLIDER_ColorWash_Count;
		static const long ID_STATICTEXT32;
		static const long ID_CHECKBOX_ColorWash_HFade;
		static const long ID_STATICTEXT33;
		static const long ID_CHECKBOX_ColorWash_VFade;
		static const long ID_PANEL5;
		static const long ID_STATICTEXT84;
		static const long ID_SLIDER_Fire_Height;
		static const long ID_STATICTEXT133;
		static const long ID_SLIDER_Fire_HueShift;
		static const long ID_STATICTEXT135;
		static const long ID_CHECKBOX_Fire_GrowFire;
		static const long ID_PANEL12;
		static const long ID_STATICTEXT34;
		static const long ID_SLIDER_Garlands_Type;
		static const long ID_STATICTEXT35;
		static const long ID_SLIDER_Garlands_Spacing;
		static const long ID_PANEL14;
		static const long ID_STATICTEXT36;
		static const long ID_SLIDER_Life_Count;
		static const long ID_STATICTEXT37;
		static const long ID_SLIDER_Life_Seed;
		static const long ID_PANEL16;
		static const long ID_STATICTEXT39;
		static const long ID_CHOICE_Meteors_Type;
		static const long ID_STATICTEXT129;
		static const long ID_CHOICE_Meteors_Effect;
		static const long ID_STATICTEXT41;
		static const long ID_SLIDER_Meteors_Count;
		static const long ID_STATICTEXT43;
		static const long ID_SLIDER_Meteors_Length;
		static const long ID_STATICTEXT131;
		static const long ID_SLIDER_Meteors_Swirl_Intensity;
		static const long ID_STATICTEXT106;
		static const long ID_CHECKBOX_Meteors_FallUp;
		static const long ID_PANEL18;
		static const long ID_BUTTON_PICTURES_FILENAME;
		static const long ID_TEXTCTRL_Pictures_Filename;
		static const long ID_STATICTEXT46;
		static const long ID_CHOICE_Pictures_Direction;
		static const long ID_STATICTEXT97;
		static const long ID_SLIDER_Pictures_GifSpeed;
		static const long ID_PANEL20;
		static const long ID_STATICTEXT80;
		static const long ID_SLIDER_Snowflakes_Count;
		static const long ID_STATICTEXT81;
		static const long ID_SLIDER_Snowflakes_Type;
		static const long ID_PANEL22;
		static const long ID_STATICTEXT45;
		static const long ID_SLIDER_Snowstorm_Count;
		static const long ID_STATICTEXT51;
		static const long ID_SLIDER_Snowstorm_Length;
		static const long ID_PANEL24;
		static const long ID_STATICTEXT38;
		static const long ID_SLIDER_Spirals_Count;
		static const long ID_STATICTEXT40;
		static const long ID_SLIDER_Spirals_Rotation;
		static const long ID_STATICTEXT42;
		static const long ID_SLIDER_Spirals_Thickness;
		static const long ID_STATICTEXT44;
		static const long ID_SLIDER_Spirals_Direction;
		static const long ID_CHECKBOX_Spirals_Blend;
		static const long ID_CHECKBOX_Spirals_3D;
		static const long ID_PANEL26;
		static const long ID_STATICTEXT53;
		static const long ID_TEXTCTRL_Text_Line1;
		static const long ID_STATICTEXT54;
		static const long ID_TEXTCTRL_Text_Line2;
		static const long ID_BUTTON_TEXT_FONT1;
		static const long ID_TEXTCTRL_Text_Font1;
		static const long ID_STATICTEXT79;
		static const long ID_CHOICE_Text_Dir1;
		static const long ID_STATICTEXT108;
		static const long ID_CHOICE_Text_Effect1;
		static const long ID_STATICTEXT99;
		static const long ID_CHOICE_Text_Count1;
		static const long ID_STATICTEXT101;
		static const long ID_SLIDER_Text_Position1;
		static const long ID_PANEL_Text1;
		static const long ID_BUTTON_TEXT_FONT2;
		static const long ID_TEXTCTRL_Text_Font2;
		static const long ID_STATICTEXT57;
		static const long ID_CHOICE_Text_Dir2;
		static const long ID_STATICTEXT109;
		static const long ID_CHOICE_Text_Effect2;
		static const long ID_STATICTEXT104;
		static const long ID_CHOICE_Text_Count2;
		static const long ID_STATICTEXT103;
		static const long ID_SLIDER_Text_Position2;
		static const long ID_PANEL_Text2;
		static const long ID_NOTEBOOK_Text1;
		static const long ID_PANEL7;
		static const long ID_STATICTEXT86;
		static const long ID_SLIDER_Twinkle_Count;
		static const long ID_STATICTEXT105;
		static const long ID_SLIDER_Twinkle_Steps;
		static const long ID_CHECKBOX_Twinkle_Strobe;
		static const long ID_PANEL33;
		static const long ID_STATICTEXT87;
		static const long ID_SLIDER_Tree_Branches;
		static const long ID_PANEL34;
		static const long ID_STATICTEXT88;
		static const long ID_SLIDER_Spirograph_R;
		static const long ID_STATICTEXT89;
		static const long ID_SLIDER_Spirograph_r;
		static const long ID_STATICTEXT90;
		static const long ID_SLIDER_Spirograph_d;
		static const long ID_STATICTEXT93;
		static const long ID_CHECKBOX_Spirograph_Animate;
		static const long ID_STATICTEXT91;
		static const long ID_PANEL35;
		static const long ID_STATICTEXT95;
		static const long ID_SLIDER_Fireworks_Number_Explosions;
		static const long ID_STATICTEXT92;
		static const long ID_SLIDER_Fireworks_Count;
		static const long ID_STATICTEXT94;
		static const long ID_SLIDER_Fireworks_Velocity;
		static const long ID_STATICTEXT96;
		static const long ID_SLIDER_Fireworks_Fade;
		static const long ID_PANEL36;
		static const long ID_STATICTEXT137;
		static const long ID_SLIDER_Circles_Count;
		static const long ID_STATICTEXT138;
		static const long ID_SLIDER_Circles_Size;
		static const long ID_CHECKBOX_Circles_Bounce;
		static const long ID_CHECKBOX_Circles_Radial;
		static const long ID_CHECKBOX_Circles_Radial_3D;
		static const long ID_CHECKBOX_Circles_Collide;
		static const long ID_CHECKBOX_Circles_Random_m;
		static const long ID_PANEL37;
		static const long ID_STATICTEXT1;
		static const long ID_SLIDER_Piano_Keyboard;
		static const long ID_PANEL39;
		static const long ID_CHOICEBOOK1;
		static const long ID_CHECKBOX_Palette1;
		static const long ID_BUTTON_Palette1;
		static const long ID_CHECKBOX_Palette2;
		static const long ID_BUTTON_Palette2;
		static const long ID_CHECKBOX_Palette3;
		static const long ID_BUTTON_Palette3;
		static const long ID_CHECKBOX_Palette4;
		static const long ID_BUTTON_Palette4;
		static const long ID_CHECKBOX_Palette5;
		static const long ID_BUTTON_Palette5;
		static const long ID_CHECKBOX_Palette6;
		static const long ID_BUTTON_Palette6;
		static const long ID_CHECKBOX_FitToTime;
		static const long ID_STATICTEXT61;
		static const long ID_SLIDER_Speed;
		static const long ID_TEXTCTRL_Speed;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL_Fadein;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL_Fadeout;
		//*)

	private:

		//(*Handlers(EffectsPanel)
		void OnButton_Pictures_FilenameClick(wxCommandEvent& event);
		void OnButton_Text_Font1Click(wxCommandEvent& event);
		void OnButton_Text_Font2Click(wxCommandEvent& event);
		void OnCheckBox_PaletteClick(wxCommandEvent& event);
		void OnButton_PaletteNumberClick(wxCommandEvent& event);
		void OnChoicebook1PageChanged(wxChoicebookEvent& event);
		void OnButton_PaletteClick(wxCommandEvent& event);
		void OnSlider_SpeedCmdScroll(wxScrollEvent& event);
		//*)

        void UpdateFont(wxTextCtrl* FontCtrl);
        void SetButtonColor(wxWindow* btn, const wxColour* c);
        int GetRandomSliderValue(wxSlider* slider);
        wxString GetAttrPrefix();
        wxString GetEffectStringFromWindow(wxWindow *ParentWin);

		DECLARE_EVENT_TABLE()
};

#endif
