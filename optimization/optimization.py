import numpy as np
import matplotlib.pyplot as plt


def random_parameter_search(functional, ChnLenMin, ChnLenMax, ChnLenStep, StpPctMin, StpPctMax, StpPctStep, start_date, end_date): 

    ChnLen = np.arange(ChnLenMin, ChnLenMax, ChnLenStep)
    StpPct = np.arange(StpPctMin, StpPctMax, StpPctStep)

    results = [[np.nan for i in range(len(StpPct))] for _ in range(len(ChnLen))]

    def paramater_search_rec(iChnLenMin, iChnLenMax, iStpPctMin, iStpPctMax): 

        nonlocal ChnLen
        nonlocal StpPct

        if np.abs(iChnLenMin - iChnLenMax) < 1 and np.abs(iStpPctMin - iStpPctMax) < 1:
            result = functional(iChnLenMin, iStpPctMin)
            results[iChnLenMin][iStpPctMin] = result
            return result
        elif np.abs(iChnLenMin - iChnLenMax) >= 1 and np.abs(iStpPctMin - iStpPctMax) < 1: 
            iChnLenMid = (iChnLenMax + iChnLenMin) // 2 
            iChnLen1 = np.random.uniform(iChnLenMin, iChnLenMid)
            iChnLen2 = np.random.uniform(iChnLenMid, iStpPctMax)
            maxRegion1 = - np.inf
            maxRegion2 = - np.inf
            for iChnLen in iChnLen1: 
                result = functional(ChnLen[iChnLen], StpPct[iStpPctMin])
                maxRegion1 = np.max(result, maxRegion1)
                results[iChnLenMin][iStpPctMin] = result
            for iChnLen in iChnLen2:
                result = functional(ChnLen[iChnLen], StpPct[iStpPctMin])
                maxRegion2 = np.max(result, maxRegion2)
                results[iChnLenMin][iStpPctMin] = result
            if maxRegion1 > maxRegion2: 
                return paramater_search_rec(iChnLenMid, iChnLenMax, iStpPctMin, iStpPctMax)
            else: 
                return paramater_search_rec(iChnLenMin, iChnLenMid, iStpPctMin, iStpPctMax)
        elif np.abs(iChnLenMin - iChnLenMax) >= 1 and np.abs(iStpPctMin - iStpPctMax) < 1:
            iStpPctMid = (iStpPctMax + iStpPctMin) // 2 
            iStpPct1 = np.random.uniform(iStpPctMin, iStpPctMid)
            iStpPct2 = np.random.uniform(iStpPctMid, iStpPctMax)
            maxRegion1 = - np.inf
            maxRegion2 = - np.inf
            for iStpPct in iStpPct1: 
                result = functional(ChnLen[iChnLenMin], StpPct[iStpPct])
                maxRegion1 = np.max(result, maxRegion1)
            for iStpPct in iStpPct2:
                result = functional(ChnLen[iChnLenMin], StpPct[iStpPct])
                maxRegion2 = np.max(result, maxRegion2)
            return 
        else: 
            return
            # Do both by separating the grid in four and searching only in the max 
            # If you can, make the code shorter and more readable 
            

        



