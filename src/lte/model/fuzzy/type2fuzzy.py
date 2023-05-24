#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Jun  9 12:39:05 2020

@authors:   amirarslan haghrah <arslan.haghrah@gmail.com>
            amiraslan haghrah <amiraslanhaghrah@gmail.com>
"""

import numpy
from pyit2fls import Mamdani, IT2FS_Gaussian_UncertStd,\
                     min_t_norm, max_s_norm, crisp, IT2FS_plot, TR_plot, \
                     R_IT2FS_Gaussian_UncertStd, L_IT2FS_Gaussian_UncertStd, product_t_norm, trapezoid_mf, IT2FS
from numpy import linspace , meshgrid, zeros, random, array, sign, arange
from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
from time import time

from numpy.polynomial import Polynomial, Chebyshev
from numpy.linalg import norm
from scipy.optimize import minimize, differential_evolution, basinhopping

class FuzzyHandover:
    
    Serving_Cell_Id = -1
    Serving_Cell_Measurements = []

    Neighbour_Cell_Measurements = {}        
    TimeStampA2Event = 0.120000000 + 0.06
    TimeStampA4Event = 0.120000000 + 0.06

    def __init__(self):        
        self.RSRQ_domain            = linspace (0., 34., 35)
        self.deltaRSRQ_domain       = linspace (-4., 4., 9)
        self.RLQ_domain             = linspace (0., 1., 101)
        self.HT_domain              = linspace (0., 1., 101)
        
        self.sPOOR       = R_IT2FS_Gaussian_UncertStd(self.RSRQ_domain, [20.0, 1.5, 0.5, 1.0])
        self.sFAIR       =   IT2FS_Gaussian_UncertStd(self.RSRQ_domain, [23.0, 1.5, 0.5, 1.0])
        self.sGOOD       =   IT2FS_Gaussian_UncertStd(self.RSRQ_domain, [27.0, 1.5, 0.5, 1.0])
        self.sEXCELLENT  = L_IT2FS_Gaussian_UncertStd(self.RSRQ_domain, [31.0, 1.5, 0.5, 1.0])

        self.sDEC = R_IT2FS_Gaussian_UncertStd(self.deltaRSRQ_domain, [-4.0, 2.0 , 0.25, 1.0])        
        self.sINC = L_IT2FS_Gaussian_UncertStd(self.deltaRSRQ_domain, [+4.0, 1.5 , 0.25, 1.0])
        

        self.nPOOR       = R_IT2FS_Gaussian_UncertStd(self.RSRQ_domain, [17.0, 1.0, 0.5, 1.0])
        self.nFAIR       =   IT2FS_Gaussian_UncertStd(self.RSRQ_domain, [20.0, 1.0, 0.5, 1.0])
        self.nGOOD       =   IT2FS_Gaussian_UncertStd(self.RSRQ_domain, [23.0, 1.0, 0.5, 1.0])
        self.nEXCELLENT  = L_IT2FS_Gaussian_UncertStd(self.RSRQ_domain, [26.0, 1.0, 0.5, 1.0])

        self.nDEC = R_IT2FS_Gaussian_UncertStd(self.deltaRSRQ_domain, [-4.0, 2.00 , 0.25, 1.0])        
        self.nINC = L_IT2FS_Gaussian_UncertStd(self.deltaRSRQ_domain, [+4.0, 2.00 , 0.25, 1.0])


        self.LOW        = R_IT2FS_Gaussian_UncertStd(self.RLQ_domain, [0.00, 0.20, 0.10, 1.0])
        self.AVERAGE    =   IT2FS_Gaussian_UncertStd(self.RLQ_domain, [0.50, 0.10, 0.04, 1.0])
        self.HIGH       = L_IT2FS_Gaussian_UncertStd(self.RLQ_domain, [1.00, 0.20, 0.10, 1.0])

        self.NO         = R_IT2FS_Gaussian_UncertStd(self.HT_domain, [0.00, 0.20, 0.10, 1.0])
        self.MIDDLE     =   IT2FS_Gaussian_UncertStd(self.HT_domain, [0.50, 0.10, 0.04, 1.0])
        self.YES        = L_IT2FS_Gaussian_UncertStd(self.HT_domain, [1.00, 0.20, 0.10, 1.0])

        if __name__ == "__main__":
            IT2FS_plot(self.sPOOR, self.sFAIR, self.sGOOD, self.sEXCELLENT, 
                        legends=["POOR", "FAIR", "GOOD", "EXCELLENT"], 
                        filename="sRSRQ_Set", xlabel="RSRQ")
            IT2FS_plot( self.sDEC,  self.sINC,
                        legends=["DEC", "INC"],
                        filename="sDeltaRSRQ_Set", xlabel="$\Delta$RSRQ")

            IT2FS_plot(self.nPOOR, self.nFAIR, self.nGOOD, self.nEXCELLENT, 
                        legends=["POOR", "FAIR", "GOOD", "EXCELLENT"], 
                        filename="nRSRQ_Set", xlabel="RSRQ")
            IT2FS_plot( self.nDEC,  self.nINC,
                        legends=["DEC", "INC"],
                        filename="nDeltaRSRQ_Set", xlabel="$\Delta$RSRQ")

            IT2FS_plot(self.LOW, self.AVERAGE, self.HIGH,
                        legends=["LOW", "AVERAGE", "HIGH"], 
                        filename="RLQ_Set", xlabel="RLQ")   

            IT2FS_plot(self.LOW, self.AVERAGE, self.HIGH,
                        legends=["LOW", "AVERAGE", "HIGH"], 
                        filename="sRLQ_Set", xlabel="Serving Cell RLQ") 

            IT2FS_plot(self.LOW, self.AVERAGE, self.HIGH,
                        legends=["LOW", "AVERAGE", "HIGH"], 
                        filename="nRLQ_Set", xlabel="Neighbor Cell RLQ") 

            IT2FS_plot(self.NO,self.MIDDLE,self.YES,
                        legends=["NO", "MIDDLE", "YES"], 
                        filename="HT_Set", xlabel="Handover Trigger") 

        # %% Serving Cell Radio Link Quality Estimator System
        self.servingCellRlqEvaluator = Mamdani(min_t_norm, max_s_norm, method='CoSet')
        self.servingCellRlqEvaluator.add_input_variable("RSRQ")
        self.servingCellRlqEvaluator.add_input_variable("deltaRSRQ")        
        self.servingCellRlqEvaluator.add_output_variable("RLQ")

        self.servingCellRlqEvaluator.add_rule([("RSRQ", self.sPOOR),      ("deltaRSRQ", self.sDEC)], [("RLQ", self.LOW)]) 
        self.servingCellRlqEvaluator.add_rule([("RSRQ", self.sPOOR),      ("deltaRSRQ", self.sINC)], [("RLQ", self.LOW)])
        self.servingCellRlqEvaluator.add_rule([("RSRQ", self.sFAIR),      ("deltaRSRQ", self.sDEC)], [("RLQ", self.LOW)])
        self.servingCellRlqEvaluator.add_rule([("RSRQ", self.sFAIR),      ("deltaRSRQ", self.sINC)], [("RLQ", self.AVERAGE)])
        self.servingCellRlqEvaluator.add_rule([("RSRQ", self.sGOOD),      ("deltaRSRQ", self.sDEC)], [("RLQ", self.AVERAGE)])
        self.servingCellRlqEvaluator.add_rule([("RSRQ", self.sGOOD),      ("deltaRSRQ", self.sINC)], [("RLQ", self.HIGH)])
        self.servingCellRlqEvaluator.add_rule([("RSRQ", self.sEXCELLENT), ("deltaRSRQ", self.sDEC)], [("RLQ", self.HIGH)])
        self.servingCellRlqEvaluator.add_rule([("RSRQ", self.sEXCELLENT), ("deltaRSRQ", self.sINC)], [("RLQ", self.HIGH)]) 
        
        # %% Neighbor Cell Radio Link Quality Estimator System
        self.neighborCellRlqEvaluator = Mamdani(min_t_norm, max_s_norm, method='CoSet')
        self.neighborCellRlqEvaluator.add_input_variable("RSRQ")
        self.neighborCellRlqEvaluator.add_input_variable("deltaRSRQ")        
        self.neighborCellRlqEvaluator.add_output_variable("RLQ")

        self.neighborCellRlqEvaluator.add_rule([("RSRQ", self.nPOOR),      ("deltaRSRQ", self.nDEC)], [("RLQ", self.LOW)]) 
        self.neighborCellRlqEvaluator.add_rule([("RSRQ", self.nPOOR),      ("deltaRSRQ", self.nINC)], [("RLQ", self.LOW)])
        self.neighborCellRlqEvaluator.add_rule([("RSRQ", self.nFAIR),      ("deltaRSRQ", self.nDEC)], [("RLQ", self.LOW)])
        self.neighborCellRlqEvaluator.add_rule([("RSRQ", self.nFAIR),      ("deltaRSRQ", self.nINC)], [("RLQ", self.AVERAGE)])
        self.neighborCellRlqEvaluator.add_rule([("RSRQ", self.nGOOD),      ("deltaRSRQ", self.nDEC)], [("RLQ", self.AVERAGE)])
        self.neighborCellRlqEvaluator.add_rule([("RSRQ", self.nGOOD),      ("deltaRSRQ", self.nINC)], [("RLQ", self.HIGH)])
        self.neighborCellRlqEvaluator.add_rule([("RSRQ", self.nEXCELLENT), ("deltaRSRQ", self.nDEC)], [("RLQ", self.HIGH)])
        self.neighborCellRlqEvaluator.add_rule([("RSRQ", self.nEXCELLENT), ("deltaRSRQ", self.nINC)], [("RLQ", self.HIGH)]) 
        


        self.handoverTriggerEvaluation = Mamdani(min_t_norm, max_s_norm, method='CoSet')
        self.handoverTriggerEvaluation.add_input_variable("sRLQ")
        self.handoverTriggerEvaluation.add_input_variable("nRLQ")
        self.handoverTriggerEvaluation.add_output_variable("hT")

        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.LOW),     ("sRLQ", self.LOW)],       [("hT", self.MIDDLE)])
        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.LOW),     ("sRLQ", self.AVERAGE)],   [("hT", self.NO)])
        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.LOW),     ("sRLQ", self.HIGH)],      [("hT", self.NO)])
        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.AVERAGE), ("sRLQ", self.LOW)],       [("hT", self.YES)])
        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.AVERAGE), ("sRLQ", self.AVERAGE)],   [("hT", self.MIDDLE)])
        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.AVERAGE), ("sRLQ", self.HIGH)],      [("hT", self.NO)])
        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.HIGH),    ("sRLQ", self.LOW)],       [("hT", self.YES)])
        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.HIGH),    ("sRLQ", self.AVERAGE)],   [("hT", self.MIDDLE)])
        self.handoverTriggerEvaluation.add_rule([("nRLQ", self.HIGH),    ("sRLQ", self.HIGH)],      [("hT", self.NO)])

    def evaluateServingCellRlq(self, rsrq, deltaRsrq):
        tr = self.servingCellRlqEvaluator.evaluate({"RSRQ":rsrq, "deltaRSRQ":deltaRsrq})           
        return crisp(tr["RLQ"])        

    def evaluateNeighborCellRlq(self, rsrq, deltaRsrq):
        tr = self.neighborCellRlqEvaluator.evaluate({"RSRQ":rsrq, "deltaRSRQ":deltaRsrq})           
        return crisp(tr["RLQ"]) 

    def evaluateHandoverTrigger (self, servingCellRlq, neighborCellRlq):
        tr = self.handoverTriggerEvaluation.evaluate({"nRLQ":neighborCellRlq, "sRLQ":servingCellRlq})
        print ('\nhT: ', '{:>6.4f}'.format(crisp(tr["hT"])), '\n')
        if crisp(tr["hT"]) >= 0.75:
            return True
        return False

    def UpdateServingCellId(self, time, imsi, cellid, rnti):
        FuzzyHandover.Serving_Cell_Id = cellid
        FuzzyHandover.Serving_Cell_Measurements = []
        if cellid in FuzzyHandover.Neighbour_Cell_Measurements:
            del FuzzyHandover.Neighbour_Cell_Measurements[cellid]

    def UpdateNeighbourMeasurements (self, time, rnti, cellId, rsrp, rsrq):       
        if cellId not in FuzzyHandover.Neighbour_Cell_Measurements:
            FuzzyHandover.Neighbour_Cell_Measurements[cellId] = []
        
        FuzzyHandover.Neighbour_Cell_Measurements[cellId].append ([time, rsrp, rsrq])        

    def EvaluateHandover (self, time, rnti, rsrp, rsrq):
        FuzzyHandover.Serving_Cell_Measurements.append ([time, rsrp, rsrq])
        
        rsrqSeries = [_rs[2] for _rs in FuzzyHandover.Serving_Cell_Measurements]        
        rsrqPred = self.servingCellRlqPredictor(rsrqSeries)        
        servingCellRlq = self.evaluateServingCellRlq (rsrqPred, rsrqPred - rsrqSeries[-1])
        print ('--- Start -----------------------------')
        print (FuzzyHandover.Serving_Cell_Id,'\t', rsrqSeries[-6:], '\t', '{:>6.2f}'.format(rsrqPred), '\t', '{:>4.2f}'.format(rsrqPred - rsrqSeries[-1]), '\t', '{:>4.2f}'.format(servingCellRlq), '\n')

        bestNeighborCellId = 0
        bestNeighborCellRlq = 0        
        for cellId in FuzzyHandover.Neighbour_Cell_Measurements.copy ():                
            if time - FuzzyHandover.Neighbour_Cell_Measurements[cellId][-1][0] > 2 * FuzzyHandover.TimeStampA4Event or FuzzyHandover.Neighbour_Cell_Measurements[cellId][-1][2] == 0:
                del FuzzyHandover.Neighbour_Cell_Measurements[cellId]                
                continue

            rsrqSeries = [_rs[2] for _rs in FuzzyHandover.Neighbour_Cell_Measurements[cellId]]                
            rsrqPred = self.neighorCellRlqPredictor(rsrqSeries)
            
            neighborCellRlq = self.evaluateNeighborCellRlq (rsrqPred, rsrqPred - rsrqSeries[-1])
            print (cellId, '\t', rsrqSeries[-6:], '\t', '{:>6.2f}'.format(rsrqPred), '\t', '{:>4.2f}'.format(rsrqPred - rsrqSeries[-1]), '\t', '{:>4.2f}'.format(neighborCellRlq))
            if bestNeighborCellRlq <= neighborCellRlq:
                bestNeighborCellId = cellId
                bestNeighborCellRlq = neighborCellRlq 

        if bestNeighborCellId > 0:
            if self.evaluateHandoverTrigger (servingCellRlq, bestNeighborCellRlq):
                return bestNeighborCellId
        return 0

    def error(self, c, t, xi):
        p = Chebyshev(c)
        x = p(t)
        e = norm((x - xi))
        return e

    def servingCellRlqPredictor(self, series):
        series = series[-6:]
            
        t = linspace(0., len(series) - 1, len(series))
        if len(series) < 2:
            c0 = [series[0] + 1]
            return Chebyshev(c0)(len(series))
        elif len(series) < 4:
            c0 = random.rand(2) - 0.5
        else:
            c0 = random.rand(3) - 0.5

        res = minimize(self.error, c0, args=(t, series, ))
        return Chebyshev(res.x)(len(series))

    def neighorCellRlqPredictor(self, series):
        series = series[-6:]
            
        t = linspace(0., len(series) - 1, len(series))
        if len(series) < 2:
            c0 = [series[0]]            
        elif len(series) < 4:            
            c0 = random.rand(2) - 0.5
        else:
            c0 = random.rand(3) - 0.5

        res = minimize(self.error, c0, args=(t, series, ))
        return Chebyshev(res.x)(len(series))

if __name__ == "__main__":    
    FH = FuzzyHandover()
    print (FH.evaluateServingCellRlq (23.20, -1.80))
    print (FH.evaluateNeighborCellRlq (23.80, 0.80))
    print (FH.evaluateNeighborCellRlq (26.0, 0.0))
    print (FH.evaluateHandoverTrigger (.20, 0.54))
    print (FH.evaluateHandoverTrigger (.21, 0.72))
    print (FH.evaluateHandoverTrigger (.27, 0.68))