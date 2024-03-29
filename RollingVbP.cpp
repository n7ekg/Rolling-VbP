#include <iterator>
#include <vector>

#include "sierrachart.h"
#include "scstudyfunctions.h"
// #include "scstudyfunctions.cpp"

/* This study resets a drawn VbP study to only consider a certain number of bars. This provides a rolling volume profile
   that will not take into consideration older data.
   
   If you don't set the number of bars back, the study will simply set the end date and time to be current. This helps
   when you accidentally move the endpoint from the current date-time to something earlier.
   
   Copyright 2019, by Ed Carp. All rights reserved. Distribution without permission is prohibited.
*/

SCDLLName("Rolling VbP v0.2")

/*==========================================================================*/
SCSFExport scsf_Rolling_VbP(SCStudyInterfaceRef sc)
{
	SCInputRef InputVbPStudy = sc.Input[0]; // volume by price study
	SCInputRef NumberOfBarsVisible = sc.Input[1];
	SCInputRef ZigZagStudy = sc.Input[2];
	SCInputRef UseZigZag = sc.Input[3];

	//s_UseTool Tool;
	/*
	const char *ChartSymbol;
	char ChartSymbolShort[16];
	const char *ptr;
	*/
	char scratchmsg[255];

	if (sc.HideStudy == 1 || sc.GetBarHasClosedStatus(sc.Index) == BHCS_BAR_HAS_NOT_CLOSED)
		return;
	
	// sc.AddMessageToLog("Startup", 1);
	if (sc.SetDefaults)
	{
		sc.GraphName = "Rolling VbP v0.2";
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.ScaleRangeType = SCALE_SAMEASREGION;
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;
		sc.AutoLoop = 1;  //Auto looping is enabled.
		sc.CalculationPrecedence = LOW_PREC_LEVEL; // Set this when using GetStudyArrayUsingID()
				
		InputVbPStudy.Name = "VbP Study";
		InputVbPStudy.SetStudySubgraphValues(1, 0);

		ZigZagStudy.Name = "Zig-Zag Study";
		ZigZagStudy.SetStudySubgraphValues(1, 0);
		
		NumberOfBarsVisible.Name = "Number Of Bars Visible";
		NumberOfBarsVisible.SetInt(10);
		NumberOfBarsVisible.SetIntLimits(0,2000);
		
		UseZigZag.Name = "Use Zig-Zag Study?";
		UseZigZag.SetYesNo(1);

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
	SCFloatArray ZigZagData;
	int BarsBack = 1;
	float Zero = 0;
	double date, time, backdate, backtime;

	SCDateTime BarDateTime = sc.BaseDateTimeIn[Index];
	BarsBack = NumberOfBarsVisible.GetInt();
	
	backdate = sc.BaseDateTimeIn[sc.Index - BarsBack].GetDate();
	backtime = std::modf(sc.BaseDateTimeIn[sc.Index - BarsBack].GetAsDouble(), &date);
	date = sc.BaseDateTimeIn[sc.Index].GetDate();
	time = std::modf(sc.BaseDateTimeIn[sc.Index].GetAsDouble(), &date);
	
	sc.GetStudyArrayUsingID(InputVbPStudy.GetStudyID(), InputVbPStudy.GetSubgraphIndex(), StudyPrice);
	sc.GetStudyArrayUsingID(ZigZagStudy.GetStudyID(), ZigZagStudy.GetSubgraphIndex(), ZigZagData);
	// BarsBack = ZigZagData[sc.Index];
	// for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	
	/*	
	for (int Index = sc.ArraySize; Index > 0; Index--)
	{
		if(ZigZagData[Index] > 0)
		{
			BarsBack = ZigZagData[Index];
			Index = 0;
		}
	}
	*/
	if(UseZigZag.GetYesNo() == 1) // Use ZigZag = Yes
	{
		if(ZigZagData[sc.Index] > 0L) BarsBack = (int)ZigZagData[sc.Index] / 2;
		NumberOfBarsVisible.SetInt(BarsBack);
		/*
		sprintf(scratchmsg, "ZZData=%f, BB=%d, USI=%d, AS=%d, IND=%d\n", ZigZagData[sc.Index], BarsBack, sc.UpdateStartIndex, sc.ArraySize, sc.Index);
		sc.AddMessageToLog(scratchmsg, 1);
		*/
	}
	
	/*
    sprintf(scratchmsg, "ZZData=%f, BB=%f, USI=%d, AS=%d, IND=%d\n", ZigZagData[sc.Index], BarsBack, sc.UpdateStartIndex, sc.ArraySize, sc.Index);
    sc.AddMessageToLog(scratchmsg, 1);
	*/

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
