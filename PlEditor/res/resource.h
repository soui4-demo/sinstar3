//stamp:10890bd2af2da119
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#pragma once
#include <res.mgr/snamedvalue.h>
#define ROBJ_IN_CPP \
namespace SOUI \
{\
    const _R R;\
    const _UIRES UIRES;\
}
namespace SOUI
{
	class _UIRES{
		public:
		class _UIDEF{
			public:
			_UIDEF(){
				XML_INIT = _T("UIDEF:XML_INIT");
			}
			const TCHAR * XML_INIT;
		}UIDEF;
		class _LAYOUT{
			public:
			_LAYOUT(){
				XML_MAINWND = _T("LAYOUT:XML_MAINWND");
				dlg_group = _T("LAYOUT:dlg_group");
				dlg_about = _T("LAYOUT:dlg_about");
			}
			const TCHAR * XML_MAINWND;
			const TCHAR * dlg_group;
			const TCHAR * dlg_about;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
		}values;
		class _IMG{
			public:
			_IMG(){
				PNG_TAB_LEFT = _T("IMG:PNG_TAB_LEFT");
				png_item_bg = _T("IMG:png_item_bg");
			}
			const TCHAR * PNG_TAB_LEFT;
			const TCHAR * png_item_bg;
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
		class _smenu{
			public:
			_smenu(){
				MENU_FILE = _T("smenu:MENU_FILE");
				MENU_EDIT = _T("smenu:MENU_EDIT");
				MENU_HELP = _T("smenu:MENU_HELP");
			}
			const TCHAR * MENU_FILE;
			const TCHAR * MENU_EDIT;
			const TCHAR * MENU_HELP;
		}smenu;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"_name_start",65535},
		{L"btn_close",65539},
		{L"btn_import_wordrate",65553},
		{L"btn_load_wordrate",65552},
		{L"btn_max",65537},
		{L"btn_min",65536},
		{L"btn_restore",65538},
		{L"btn_saveas_txt",65554},
		{L"btn_saveas_wordrate",65555},
		{L"chk_group_name",65542},
		{L"col_editor",65545},
		{L"col_group",65541},
		{L"col_remark",65547},
		{L"col_size",65543},
		{L"edit_group_editor",65558},
		{L"edit_group_name",65557},
		{L"edit_group_remark",65559},
		{L"edit_rate_default",65551},
		{L"edit_rate_max",65550},
		{L"edit_rate_min",65549},
		{L"lv_plgroup",65540},
		{L"menu_about",102},
		{L"menu_add_group",21},
		{L"menu_clear_group",24},
		{L"menu_del_group",23},
		{L"menu_exit",12},
		{L"menu_export_from_group",34},
		{L"menu_help",101},
		{L"menu_import_to_group",33},
		{L"menu_load_rate_db",11},
		{L"menu_modify_group",22},
		{L"menu_open_lib",13},
		{L"menu_save_lib",14},
		{L"prog_import",65556},
		{L"txt_editor",65546},
		{L"txt_group_size",65544},
		{L"txt_remark",65548}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			btn_close = namedXmlID[1].strName;
			btn_import_wordrate = namedXmlID[2].strName;
			btn_load_wordrate = namedXmlID[3].strName;
			btn_max = namedXmlID[4].strName;
			btn_min = namedXmlID[5].strName;
			btn_restore = namedXmlID[6].strName;
			btn_saveas_txt = namedXmlID[7].strName;
			btn_saveas_wordrate = namedXmlID[8].strName;
			chk_group_name = namedXmlID[9].strName;
			col_editor = namedXmlID[10].strName;
			col_group = namedXmlID[11].strName;
			col_remark = namedXmlID[12].strName;
			col_size = namedXmlID[13].strName;
			edit_group_editor = namedXmlID[14].strName;
			edit_group_name = namedXmlID[15].strName;
			edit_group_remark = namedXmlID[16].strName;
			edit_rate_default = namedXmlID[17].strName;
			edit_rate_max = namedXmlID[18].strName;
			edit_rate_min = namedXmlID[19].strName;
			lv_plgroup = namedXmlID[20].strName;
			menu_about = namedXmlID[21].strName;
			menu_add_group = namedXmlID[22].strName;
			menu_clear_group = namedXmlID[23].strName;
			menu_del_group = namedXmlID[24].strName;
			menu_exit = namedXmlID[25].strName;
			menu_export_from_group = namedXmlID[26].strName;
			menu_help = namedXmlID[27].strName;
			menu_import_to_group = namedXmlID[28].strName;
			menu_load_rate_db = namedXmlID[29].strName;
			menu_modify_group = namedXmlID[30].strName;
			menu_open_lib = namedXmlID[31].strName;
			menu_save_lib = namedXmlID[32].strName;
			prog_import = namedXmlID[33].strName;
			txt_editor = namedXmlID[34].strName;
			txt_group_size = namedXmlID[35].strName;
			txt_remark = namedXmlID[36].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * btn_close;
		 const wchar_t * btn_import_wordrate;
		 const wchar_t * btn_load_wordrate;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_restore;
		 const wchar_t * btn_saveas_txt;
		 const wchar_t * btn_saveas_wordrate;
		 const wchar_t * chk_group_name;
		 const wchar_t * col_editor;
		 const wchar_t * col_group;
		 const wchar_t * col_remark;
		 const wchar_t * col_size;
		 const wchar_t * edit_group_editor;
		 const wchar_t * edit_group_name;
		 const wchar_t * edit_group_remark;
		 const wchar_t * edit_rate_default;
		 const wchar_t * edit_rate_max;
		 const wchar_t * edit_rate_min;
		 const wchar_t * lv_plgroup;
		 const wchar_t * menu_about;
		 const wchar_t * menu_add_group;
		 const wchar_t * menu_clear_group;
		 const wchar_t * menu_del_group;
		 const wchar_t * menu_exit;
		 const wchar_t * menu_export_from_group;
		 const wchar_t * menu_help;
		 const wchar_t * menu_import_to_group;
		 const wchar_t * menu_load_rate_db;
		 const wchar_t * menu_modify_group;
		 const wchar_t * menu_open_lib;
		 const wchar_t * menu_save_lib;
		 const wchar_t * prog_import;
		 const wchar_t * txt_editor;
		 const wchar_t * txt_group_size;
		 const wchar_t * txt_remark;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int btn_close	=	65539;
		const static int btn_import_wordrate	=	65553;
		const static int btn_load_wordrate	=	65552;
		const static int btn_max	=	65537;
		const static int btn_min	=	65536;
		const static int btn_restore	=	65538;
		const static int btn_saveas_txt	=	65554;
		const static int btn_saveas_wordrate	=	65555;
		const static int chk_group_name	=	65542;
		const static int col_editor	=	65545;
		const static int col_group	=	65541;
		const static int col_remark	=	65547;
		const static int col_size	=	65543;
		const static int edit_group_editor	=	65558;
		const static int edit_group_name	=	65557;
		const static int edit_group_remark	=	65559;
		const static int edit_rate_default	=	65551;
		const static int edit_rate_max	=	65550;
		const static int edit_rate_min	=	65549;
		const static int lv_plgroup	=	65540;
		const static int menu_about	=	102;
		const static int menu_add_group	=	21;
		const static int menu_clear_group	=	24;
		const static int menu_del_group	=	23;
		const static int menu_exit	=	12;
		const static int menu_export_from_group	=	34;
		const static int menu_help	=	101;
		const static int menu_import_to_group	=	33;
		const static int menu_load_rate_db	=	11;
		const static int menu_modify_group	=	22;
		const static int menu_open_lib	=	13;
		const static int menu_save_lib	=	14;
		const static int prog_import	=	65556;
		const static int txt_editor	=	65546;
		const static int txt_group_size	=	65544;
		const static int txt_remark	=	65548;
		}id;

		class _string{
		public:
		const static int about	=	0;
		const static int copy_right	=	1;
		const static int group_info	=	2;
		const static int help	=	3;
		const static int title	=	4;
		const static int ver	=	5;
		const static int website	=	6;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int gray	=	1;
		const static int green	=	2;
		const static int red	=	3;
		const static int white	=	4;
		}color;

	};

#ifdef R_IN_CPP
	 extern const _R R;
	 extern const _UIRES UIRES;
#else
	 extern const __declspec(selectany) _R & R = _R();
	 extern const __declspec(selectany) _UIRES & UIRES = _UIRES();
#endif//R_IN_CPP
}
