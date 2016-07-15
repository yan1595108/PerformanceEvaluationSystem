// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


// Dispatch interfaces referenced by this interface
class COleFont;

/////////////////////////////////////////////////////////////////////////////
// CiPlotAxisX wrapper class

class CiPlotAxisX : public COleDispatchDriver
{
public:
	CiPlotAxisX() {}		// Calls COleDispatchDriver default constructor
	CiPlotAxisX(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CiPlotAxisX(const CiPlotAxisX& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetName();
	void SetName(LPCTSTR lpszNewValue);
	BOOL GetVisible();
	void SetVisible(BOOL bNewValue);
	double GetStartPercent();
	void SetStartPercent(double newValue);
	double GetStopPercent();
	void SetStopPercent(double newValue);
	double GetMin();
	void SetMin(double newValue);
	double GetSpan();
	void SetSpan(double newValue);
	double GetMax();
	BOOL GetReverseScale();
	void SetReverseScale(BOOL bNewValue);
	long GetInnerMargin();
	void SetInnerMargin(long nNewValue);
	long GetOuterMargin();
	void SetOuterMargin(long nNewValue);
	CString GetTitle();
	void SetTitle(LPCTSTR lpszNewValue);
	double GetTitleMargin();
	void SetTitleMargin(double newValue);
	COleFont GetTitleFont();
	void SetTitleFont(LPDISPATCH newValue);
	BOOL GetTitleShow();
	void SetTitleShow(BOOL bNewValue);
	long GetMajorLength();
	void SetMajorLength(long nNewValue);
	long GetMinorLength();
	void SetMinorLength(long nNewValue);
	long GetMinorCount();
	void SetMinorCount(long nNewValue);
	double GetLabelsMargin();
	void SetLabelsMargin(double newValue);
	COleFont GetLabelsFont();
	void SetLabelsFont(LPDISPATCH newValue);
	long GetLabelsPrecisionStyle();
	void SetLabelsPrecisionStyle(long nNewValue);
	long GetLabelsPrecision();
	void SetLabelsPrecision(long nNewValue);
	double GetLabelsMinLength();
	void SetLabelsMinLength(double newValue);
	double GetLabelSeparation();
	void SetLabelSeparation(double newValue);
	long GetLabelsFormatStyle();
	void SetLabelsFormatStyle(long nNewValue);
	CString GetDateTimeFormat();
	void SetDateTimeFormat(LPCTSTR lpszNewValue);
	BOOL GetScaleLineShow();
	void SetScaleLineShow(BOOL bNewValue);
	unsigned long GetScaleLinesColor();
	void SetScaleLinesColor(unsigned long newValue);
	double GetStackingEndsMargin();
	void SetStackingEndsMargin(double newValue);
	long GetScaleType();
	void SetScaleType(long nNewValue);
	BOOL GetTrackingEnabled();
	void SetTrackingEnabled(BOOL bNewValue);
	long GetTrackingStyle();
	void SetTrackingStyle(long nNewValue);
	long GetTrackingAlignFirstStyle();
	void SetTrackingAlignFirstStyle(long nNewValue);
	double GetTrackingScrollCompressMax();
	void SetTrackingScrollCompressMax(double newValue);
	CString GetLabelText(double Value);
	void NewTrackingData(double Value);
	void ResetFirstAlign();
	void Zoom(double Value);
	long PositionToPixels(double Value);
	double PixelsToPosition(long Value);
	BOOL ValueOnScale(double Value);
	long GetZOrder();
	void SetZOrder(long nNewValue);
	BOOL GetHorizontal();
	void SetHorizontal(BOOL bNewValue);
	unsigned long GetTitleFontColor();
	void SetTitleFontColor(unsigned long newValue);
	unsigned long GetLabelsFontColor();
	void SetLabelsFontColor(unsigned long newValue);
	BOOL GetLabelsMinLengthAutoAdjust();
	void SetLabelsMinLengthAutoAdjust(BOOL bNewValue);
	void UpdateResumeValues();
	double GetDesiredIncrement();
	void SetDesiredIncrement(double newValue);
	long GetCursorPrecision();
	void SetCursorPrecision(long nNewValue);
	double GetCursorScaler();
	void SetCursorScaler(double newValue);
	BOOL GetEnabled();
	void SetEnabled(BOOL bNewValue);
	BOOL GetPopupEnabled();
	void SetPopupEnabled(BOOL bNewValue);
	BOOL GetRestoreValuesOnResume();
	void SetRestoreValuesOnResume(BOOL bNewValue);
	long GetWidth();
	long GetHeight();
	BOOL GetScrollMinMaxEnabled();
	void SetScrollMinMaxEnabled(BOOL bNewValue);
	double GetScrollMax();
	void SetScrollMax(double newValue);
	double GetScrollMin();
	void SetScrollMin(double newValue);
};
