// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


// Dispatch interfaces referenced by this interface
class CiPlotAxisX;
class CiPlotLegendX;
class CiPlotToolBarX;
class CiPlotDataViewX;
class COleFont;
class CiPlotAnnotationX;
class CPicture;
class CiPlotDataCursorX;
class CiPlotLimitX;
class CiXYPlotChannelX;

/////////////////////////////////////////////////////////////////////////////
// CiXYPlotX wrapper class

class CiXYPlotX : public CWnd
{
protected:
	DECLARE_DYNCREATE(CiXYPlotX)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xd1cae8f4, 0x6dff, 0x4187, { 0xb1, 0xb8, 0xdd, 0xcf, 0x91, 0xf9, 0x8a, 0x8a } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:

// Operations
public:
	long GetDataViewZHorz();
	void SetDataViewZHorz(long nNewValue);
	long GetDataViewZVert();
	void SetDataViewZVert(long nNewValue);
	BOOL GetXYAxesReverse();
	void SetXYAxesReverse(BOOL bNewValue);
	long GetOuterMarginLeft();
	void SetOuterMarginLeft(long nNewValue);
	long GetOuterMarginTop();
	void SetOuterMarginTop(long nNewValue);
	long GetOuterMarginRight();
	void SetOuterMarginRight(long nNewValue);
	long GetOuterMarginBottom();
	void SetOuterMarginBottom(long nNewValue);
	long GetPrintOrientation();
	void SetPrintOrientation(long nNewValue);
	double GetPrintMarginLeft();
	void SetPrintMarginLeft(double newValue);
	double GetPrintMarginTop();
	void SetPrintMarginTop(double newValue);
	double GetPrintMarginRight();
	void SetPrintMarginRight(double newValue);
	double GetPrintMarginBottom();
	void SetPrintMarginBottom(double newValue);
	BOOL GetPrintShowDialog();
	void SetPrintShowDialog(BOOL bNewValue);
	long GetUpdateFrameRate();
	void SetUpdateFrameRate(long nNewValue);
	unsigned long GetBackGroundColor();
	void SetBackGroundColor(unsigned long newValue);
	long GetBorderStyle();
	void SetBorderStyle(long nNewValue);
	BOOL GetAutoFrameRate();
	void SetAutoFrameRate(BOOL bNewValue);
	void AboutBox();
	CiPlotAxisX GetXAxis(long Index);
	CiPlotAxisX GetYAxis(long Index);
	CiPlotLegendX GetLegend(long Index);
	CiPlotToolBarX GetToolBar(long Index);
	CiPlotDataViewX GetDataView(long Index);
	void DisableLayoutManager();
	void EnableLayoutManager();
	void DisableAllTracking();
	void EnableAllTracking();
	long AddXAxis();
	long AddYAxis();
	long AddLegend();
	long AddToolBar();
	long AddDataView();
	long AddChannel();
	void RemoveAllXAxes();
	void RemoveAllYAxes();
	void RemoveAllLegends();
	void RemoveAllToolBars();
	void RemoveAllDataViews();
	void RemoveAllChannels();
	long GetXAxisCount();
	long GetYAxisCount();
	long GetLegendCount();
	long GetToolBarCount();
	long GetDataViewCount();
	long GetChannelCount();
	BOOL GetHintsShow();
	void SetHintsShow(BOOL bNewValue);
	long GetHintsPause();
	void SetHintsPause(long nNewValue);
	long GetHintsHidePause();
	void SetHintsHidePause(long nNewValue);
	void DeleteToolBar(long Index);
	void DeleteLegend(long Index);
	void DeleteXAxis(long Index);
	void DeleteYAxis(long Index);
	void DeleteChannel(long Index);
	void DeleteDataView(long Index);
	void ShowPropertyEditor();
	BOOL GetTitleVisible();
	void SetTitleVisible(BOOL bNewValue);
	CString GetTitleText();
	void SetTitleText(LPCTSTR lpszNewValue);
	double GetTitleMargin();
	void SetTitleMargin(double newValue);
	COleFont GetTitleFont();
	void SetTitleFont(LPDISPATCH newValue);
	unsigned long GetTitleFontColor();
	void SetTitleFontColor(unsigned long newValue);
	void RemoveAllAnnotations();
	void DeleteAnnotation(long Index);
	long AddAnnotation();
	long GetAnnotationCount();
	CiPlotAnnotationX GetAnnotation(long Index);
	long GetChannelIndexByName(LPCTSTR Value);
	long GetXAxisIndexByName(LPCTSTR Value);
	long GetYAxisIndexByName(LPCTSTR Value);
	BOOL GetUserCanEditObjects();
	void SetUserCanEditObjects(BOOL bNewValue);
	void PrintChart();
	void CopyToClipBoard();
	void Save();
	CPicture GetSnapShotPicture();
	void RepaintAll();
	void BeginUpdate();
	void EndUpdate();
	void Lock();
	void Unlock();
	void ClearAllData();
	void AddDataArray(double XValue, const VARIANT& Data);
	double GetNow();
	void SavePropertiesToFile(LPCTSTR FileName);
	void LoadPropertiesFromFile(LPCTSTR FileName);
	void SaveDataToFile(LPCTSTR FileName);
	void LoadDataFromFile(LPCTSTR FileName);
	CString GetLogFileName();
	void SetLogFileName(LPCTSTR lpszNewValue);
	long GetLogBufferSize();
	void SetLogBufferSize(long nNewValue);
	void LogActivate(BOOL Append);
	void LogDeactivate();
	BOOL GetLoggingActive();
	long GetComponentHandle();
	CiPlotDataCursorX GetDataCursor(long Index);
	long GetDataCursorCount();
	long AddDataCursor();
	void DeleteDataCursor(long Index);
	void RemoveAllDataCursors();
	CiPlotLimitX GetLimit(long Index);
	long GetLimitCount();
	long AddLimit();
	void DeleteLimit(long Index);
	void RemoveAllLimits();
	void SaveImageToBitmap(LPCTSTR FileName);
	void SaveImageToMetaFile(LPCTSTR FileName);
	void SaveImageToJPEG(LPCTSTR FileName, long Compression, BOOL Progressive);
	void SaveTranslationsToFile(LPCTSTR FileName);
	void LoadTranslationsFromFile(LPCTSTR FileName);
	void RemoveAllTranslations();
	void DeleteTranslation(long Index);
	long AddTranslation(LPCTSTR OriginalString, LPCTSTR ReplacementString);
	long TranslationCount();
	CString GetTranslationOriginalString(long Index);
	CString GetTranslationReplacementString(long Index);
	CiXYPlotChannelX GetChannel(long Index);
};
