#include <iterator>
#include <vector>

#include "sierrachart.h"
#include "scstudyfunctions.h"
// #include "scstudyfunctions.cpp"

/* This study resets a drawn VbP study to only consider a certain number of bars. This provides a rolling volume profile
   that will not take into consideration older data.
   
   If you don't set the number of bars back, the study will simply set the end date and time to be current. This helps
   when you accidentally move the endpoint from the current date-time to something earlier.
*/

SCDLLName("Rolling VbP v0.1c")

/*==========================================================================*/
SCSFExport scsf_Rolling_VbP(SCStudyInterfaceRef sc)
{
	SCInputRef InputVbPStudy = sc.Input[0]; // volume by price study
	SCInputRef NumberOfBarsVisible = sc.Input[1];

	//s_UseTool Tool;
	/*
	const char *ChartSymbol;
	char ChartSymbolShort[16];
	const char *ptr;
	*/
	char scratchmsg[255];

	if (sc.HideStudy == 1)
		return;
	
	// sc.AddMessageToLog("Startup", 1);
	if (sc.SetDefaults)
	{
		sc.GraphName = "Rolling VbP v0.1c";
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.ScaleRangeType = SCALE_SAMEASREGION;
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;
		sc.AutoLoop = 1;  //Auto looping is enabled.
		sc.CalculationPrecedence = LOW_PREC_LEVEL; // Set this when using GetStudyArrayUsingID()
				
		InputVbPStudy.Name = "VbP Study";
		InputVbPStudy.SetStudySubgraphValues(1, 0);
		
		NumberOfBarsVisible.Name = "Number Of Bars Visible";
		NumberOfBarsVisible.SetInt(10);
		NumberOfBarsVisible.SetIntLimits(0,2000);

		return;
	}

	int IntradayChartDate = 0;
	int NumberOfForwardBars = 0;
	int IsValid = 1;

	// we get chart data only once for speed
	SCGraphData DailyChartData;
	SCDateTimeArray DailyChartDateTimes;
	sc.DataStartIndex = 0;
	sc.GetChartBaseData(1, DailyChartData);
	sc.GetChartDateTimeArray(1, DailyChartDateTimes);
	
	//int StartIndex = sc.UpdateStartIndex;

	int Index = sc.ArraySize - 1;
	SCFloatArray StudyPrice;
	int BarsBack = 1;
	float Zero = 0;
	double date, time, backdate, backtime;

	SCDateTime BarDateTime = sc.BaseDateTimeIn[Index];
	BarsBack = NumberOfBarsVisible.GetInt();
	
	backdate = sc.BaseDateTimeIn[sc.Index - BarsBack].GetDate();
	backtime = modf(sc.BaseDateTimeIn[sc.Index - BarsBack], &date);
	date = sc.BaseDateTimeIn[sc.Index].GetDate();
	time = modf(sc.BaseDateTimeIn[sc.Index], &date);
	
	sc.GetStudyArrayUsingID(InputVbPStudy.GetStudyID(), InputVbPStudy.GetSubgraphIndex(), StudyPrice);
	// Set start date and time to what it was NumberOfBarsVisible.GetInteger() ago
	// If it's set to zero, just set the end date/time to 0 (current)
	if (BarsBack > 0)
	{
		sc.SetChartStudyInputFloat(sc.ChartNumber, InputVbPStudy.GetStudyID(), 36, (double)backdate);
		sc.SetChartStudyInputFloat(sc.ChartNumber, InputVbPStudy.GetStudyID(), 37, 1);
		sc.SetChartStudyInputFloat(sc.ChartNumber, InputVbPStudy.GetStudyID(), 38, (double)backtime);
	}
	// Set end date and time to 0 (current)
	sc.SetChartStudyInputFloat(sc.ChartNumber, InputVbPStudy.GetStudyID(), 39, (double)0);
	sc.SetChartStudyInputFloat(sc.ChartNumber, InputVbPStudy.GetStudyID(), 40, (double)0);
	// Recalculate chart
	// sc.RecalculateChart(sc.ChartNumber);
}
