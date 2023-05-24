#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Nov 11 16:33:42 2020

@authors:   amirarslan haghrah <arslan.haghrah@gmail.com>
            amiraslan haghrah <amiraslanhaghrah@gmail.com>
"""

from skfuzzy import control as ctrl
from numpy import linspace, random
from numpy.polynomial import Chebyshev
from scipy.optimize import minimize
from numpy.linalg import norm
from pyit2fls import gaussian_mf
import matplotlib.pyplot as plt

def r_gaussian_mf(x, params):
    return (x < params[0]) * params[2] + gaussian_mf(x, params) * (x >= params[0])

def l_gaussian_mf(x, params):
    return (x > params[0]) * params[2] + gaussian_mf(x, params) * (x <= params[0])

class FuzzyHandover:
    Serving_Cell_Id = -1
    Serving_Cell_Measurements = []

    Neighbour_Cell_Measurements = {}        
    TimeStampA2Event = 0.120000000
    TimeStampA4Event = 0.120000000

    def __init__(self):
        self.RSRQ_domain            = linspace ( 0., 34., 35)
        self.deltaRSRQ_domain       = linspace (-4., 4., 9)
        self.RLQ_domain             = linspace ( 0., 1., 101)
        self.HT_domain              = linspace ( 0., 1., 101)
        
        self.sRSRQ = ctrl.Antecedent(self.RSRQ_domain, "RSRQ")
        self.sDeltaRSRQ = ctrl.Antecedent(self.deltaRSRQ_domain, "deltaRSRQ")
        
        self.nRSRQ = ctrl.Antecedent(self.RSRQ_domain, "RSRQ")
        self.nDeltaRSRQ = ctrl.Antecedent(self.deltaRSRQ_domain, "deltaRSRQ")
        
        self.RLQ = ctrl.Consequent(self.RLQ_domain, "RLQ")

        self.sRLQ = ctrl.Antecedent(self.RLQ_domain, "sRLQ")
        self.nRLQ = ctrl.Antecedent(self.RLQ_domain, "nRLQ")
        self.hT = ctrl.Consequent(self.HT_domain, "hT")
        

        self.sRSRQ["POOR"] =       r_gaussian_mf(self.RSRQ_domain, [20., 1.5, 1.])
        self.sRSRQ["FAIR"] =         gaussian_mf(self.RSRQ_domain, [23., 1.5, 1.])
        self.sRSRQ["GOOD"] =         gaussian_mf(self.RSRQ_domain, [27., 1.5, 1.])
        self.sRSRQ["EXCELLENT"] =  l_gaussian_mf(self.RSRQ_domain, [31., 1.5, 1.])        
        if __name__ == "__main__":
            self.sRSRQ.view()
            plt.xlabel("RSRQ", fontsize=20)
            plt.ylabel("Membership degree", fontsize=20) 
            plt.xticks(fontsize=18)
            plt.yticks(fontsize=18)
            plt.gca().spines['right'].set_visible(True)
            plt.gca().spines['top'].set_visible(True)            
            plt.grid(True)
            plt.legend(prop={'size': 16})
            plt.savefig("sRSRQ_Set" + ".pdf", format="pdf", dpi=300, bbox_inches="tight")

        self.sDeltaRSRQ["DEC"] = r_gaussian_mf(self.deltaRSRQ_domain, [-4., 2.0, 1.])        
        self.sDeltaRSRQ["INC"] = l_gaussian_mf(self.deltaRSRQ_domain, [+4., 1.5, 1.]) 
        if __name__ == "__main__":
            self.sDeltaRSRQ.view()
            plt.xlabel("$\Delta$RSRQ", fontsize=20)
            plt.ylabel("Membership degree", fontsize=20) 
            plt.xticks(fontsize=18)
            plt.yticks(fontsize=18)
            plt.gca().spines['right'].set_visible(True)
            plt.gca().spines['top'].set_visible(True)
            plt.grid(True)
            plt.legend(prop={'size': 16})
            plt.savefig("sDeltaRSRQ_Set" + ".pdf", format="pdf", dpi=300, bbox_inches="tight")
        
        self.nRSRQ["POOR"] =       r_gaussian_mf(self.RSRQ_domain, [17., 1., 1.])
        self.nRSRQ["FAIR"] =         gaussian_mf(self.RSRQ_domain, [20., 1., 1.])
        self.nRSRQ["GOOD"] =         gaussian_mf(self.RSRQ_domain, [23., 1., 1.])
        self.nRSRQ["EXCELLENT"] =  l_gaussian_mf(self.RSRQ_domain, [26., 1., 1.])
        if __name__ == "__main__":
            self.nRSRQ.view()
            plt.xlabel("RSRQ", fontsize=20)
            plt.ylabel("Membership degree", fontsize=20) 
            plt.xticks(fontsize=18)
            plt.yticks(fontsize=18)
            plt.gca().spines['right'].set_visible(True)
            plt.gca().spines['top'].set_visible(True)
            plt.grid(True)
            plt.legend(prop={'size': 16})
            plt.savefig("nRSRQ_Set" + ".pdf", format="pdf", dpi=300, bbox_inches="tight")

        self.nDeltaRSRQ["DEC"] = r_gaussian_mf(self.deltaRSRQ_domain, [-4., 2.0, 1.])        
        self.nDeltaRSRQ["INC"] = l_gaussian_mf(self.deltaRSRQ_domain, [+4., 2.0, 1.]) 
        if __name__ == "__main__":
            self.nDeltaRSRQ.view()
            plt.xlabel("$\Delta$RSRQ", fontsize=20)
            plt.ylabel("Membership degree", fontsize=20) 
            plt.xticks(fontsize=18)
            plt.yticks(fontsize=18)
            plt.gca().spines['right'].set_visible(True)
            plt.gca().spines['top'].set_visible(True)
            plt.grid(True)
            plt.legend(prop={'size': 16})
            plt.savefig("nDeltaRSRQ_Set" + ".pdf", format="pdf", dpi=300, bbox_inches="tight")        
        
        self.RLQ["LOW"] =     r_gaussian_mf(self.RLQ_domain, [0.00, 0.20, 1.])        
        self.RLQ["AVERAGE"] =   gaussian_mf(self.RLQ_domain, [0.50, 0.10, 1.])
        self.RLQ["HIGH"] =    l_gaussian_mf(self.RLQ_domain, [1.00, 0.20, 1.])                
        if __name__ == "__main__":
            self.RLQ.view()
            plt.xlabel("RLQ", fontsize=20)
            plt.ylabel("Membership degree", fontsize=20) 
            plt.xticks(fontsize=18)
            plt.yticks(fontsize=18)            
            plt.gca().spines['right'].set_visible(True)
            plt.gca().spines['top'].set_visible(True)
            plt.grid(True)
            plt.legend(prop={'size': 16})
            plt.savefig("RLQ_Set" + ".pdf", format="pdf", dpi=300, bbox_inches="tight")

        self.sRLQ["LOW"] =     r_gaussian_mf(self.RLQ_domain, [0.00, 0.20, 1.])        
        self.sRLQ["AVERAGE"] =   gaussian_mf(self.RLQ_domain, [0.50, 0.10, 1.])
        self.sRLQ["HIGH"] =    l_gaussian_mf(self.RLQ_domain, [1.00, 0.20, 1.])                
        if __name__ == "__main__":
            self.sRLQ.view()
            plt.xlabel("Serving Cell RLQ", fontsize=20)
            plt.ylabel("Membership degree", fontsize=20) 
            plt.xticks(fontsize=18)
            plt.yticks(fontsize=18)
            plt.gca().spines['right'].set_visible(True)
            plt.gca().spines['top'].set_visible(True)
            plt.grid(True)
            plt.legend(prop={'size': 16})
            plt.savefig("sRLQ_Set" + ".pdf", format="pdf", dpi=300, bbox_inches="tight")

        self.nRLQ["LOW"] =     r_gaussian_mf(self.RLQ_domain, [0.00, 0.20, 1.])        
        self.nRLQ["AVERAGE"] =   gaussian_mf(self.RLQ_domain, [0.50, 0.10, 1.])
        self.nRLQ["HIGH"] =    l_gaussian_mf(self.RLQ_domain, [1.00, 0.20, 1.])        
        if __name__ == "__main__":
            self.nRLQ.view()
            plt.xlabel("Neighbor Cell RLQ", fontsize=20)
            plt.ylabel("Membership degree", fontsize=20) 
            plt.xticks(fontsize=18)
            plt.yticks(fontsize=18)
            plt.gca().spines['right'].set_visible(True)
            plt.gca().spines['top'].set_visible(True)
            plt.grid(True)
            plt.legend(prop={'size': 16})
            plt.savefig("nRLQ_Set" + ".pdf", format="pdf", dpi=300, bbox_inches="tight")

        self.hT["NO"]     = r_gaussian_mf(self.HT_domain, [0.00, 0.20, 1.])
        self.hT["MIDDLE"] =   gaussian_mf(self.HT_domain, [0.50, 0.10, 1.])
        self.hT["YES"]    = l_gaussian_mf(self.HT_domain, [1.00, 0.20, 1.])        
        if __name__ == "__main__":
            self.hT.view()
            plt.xlabel("Handover Trigger", fontsize=20)
            plt.ylabel("Membership degree", fontsize=20) 
            plt.xticks(fontsize=18)
            plt.yticks(fontsize=18)
            plt.gca().spines['right'].set_visible(True)
            plt.gca().spines['top'].set_visible(True)
            plt.grid(True)
            plt.legend(prop={'size': 16})
            plt.savefig("HT_Set" + ".pdf", format="pdf", dpi=300, bbox_inches="tight")
        
        sRlqRule1 = ctrl.Rule(self.sRSRQ["POOR"] &      self.sDeltaRSRQ["DEC"], self.RLQ["LOW"])        
        sRlqRule2 = ctrl.Rule(self.sRSRQ["POOR"] &      self.sDeltaRSRQ["INC"], self.RLQ["LOW"])
        sRlqRule3 = ctrl.Rule(self.sRSRQ["FAIR"] &      self.sDeltaRSRQ["DEC"], self.RLQ["LOW"])
        sRlqRule4 = ctrl.Rule(self.sRSRQ["FAIR"] &      self.sDeltaRSRQ["INC"], self.RLQ["AVERAGE"])
        sRlqRule5 = ctrl.Rule(self.sRSRQ["GOOD"] &      self.sDeltaRSRQ["DEC"], self.RLQ["AVERAGE"])
        sRlqRule6 = ctrl.Rule(self.sRSRQ["GOOD"] &      self.sDeltaRSRQ["INC"], self.RLQ["HIGH"])
        sRlqRule7 = ctrl.Rule(self.sRSRQ["EXCELLENT"] & self.sDeltaRSRQ["DEC"], self.RLQ["HIGH"])        
        sRlqRule8 = ctrl.Rule(self.sRSRQ["EXCELLENT"] & self.sDeltaRSRQ["INC"], self.RLQ["HIGH"])
        
        self.servingCellRlqEvaluation_ctrl = ctrl.ControlSystem([sRlqRule1, sRlqRule2, sRlqRule3, sRlqRule4, 
                                                                    sRlqRule5, sRlqRule6, sRlqRule7, sRlqRule8])
        self.servingCellRlqEvaluation = ctrl.ControlSystemSimulation(self.servingCellRlqEvaluation_ctrl)
        

        nRlqRule1 = ctrl.Rule(self.nRSRQ["POOR"] &      self.nDeltaRSRQ["DEC"], self.RLQ["LOW"])        
        nRlqRule2 = ctrl.Rule(self.nRSRQ["POOR"] &      self.nDeltaRSRQ["INC"], self.RLQ["LOW"])
        nRlqRule3 = ctrl.Rule(self.nRSRQ["FAIR"] &      self.nDeltaRSRQ["DEC"], self.RLQ["LOW"])
        nRlqRule4 = ctrl.Rule(self.nRSRQ["FAIR"] &      self.nDeltaRSRQ["INC"], self.RLQ["AVERAGE"])
        nRlqRule5 = ctrl.Rule(self.nRSRQ["GOOD"] &      self.nDeltaRSRQ["DEC"], self.RLQ["AVERAGE"])
        nRlqRule6 = ctrl.Rule(self.nRSRQ["GOOD"] &      self.nDeltaRSRQ["INC"], self.RLQ["HIGH"])
        nRlqRule7 = ctrl.Rule(self.nRSRQ["EXCELLENT"] & self.nDeltaRSRQ["DEC"], self.RLQ["HIGH"])        
        nRlqRule8 = ctrl.Rule(self.nRSRQ["EXCELLENT"] & self.nDeltaRSRQ["INC"], self.RLQ["HIGH"])
        
        self.neighborCellRlqEvaluation_ctrl = ctrl.ControlSystem([nRlqRule1, nRlqRule2, nRlqRule3, nRlqRule4, 
                                                                    nRlqRule5, nRlqRule6, nRlqRule7, nRlqRule8])
        self.neighborCellRlqEvaluation = ctrl.ControlSystemSimulation(self.neighborCellRlqEvaluation_ctrl)

        
        htRule1 = ctrl.Rule(self.nRLQ["LOW"] &      self.sRLQ["LOW"],       self.hT["MIDDLE"])
        htRule2 = ctrl.Rule(self.nRLQ["LOW"] &      self.sRLQ["AVERAGE"],   self.hT["NO"])
        htRule3 = ctrl.Rule(self.nRLQ["LOW"] &      self.sRLQ["HIGH"],      self.hT["NO"]) 
        htRule4 = ctrl.Rule(self.nRLQ["AVERAGE"] &  self.sRLQ["LOW"],       self.hT["YES"])
        htRule5 = ctrl.Rule(self.nRLQ["AVERAGE"] &  self.sRLQ["AVERAGE"],   self.hT["MIDDLE"])
        htRule6 = ctrl.Rule(self.nRLQ["AVERAGE"] &  self.sRLQ["HIGH"],      self.hT["NO"]) 
        htRule7 = ctrl.Rule(self.nRLQ["HIGH"] &     self.sRLQ["LOW"],       self.hT["YES"])
        htRule8 = ctrl.Rule(self.nRLQ["HIGH"] &     self.sRLQ["AVERAGE"],   self.hT["MIDDLE"])
        htRule9 = ctrl.Rule(self.nRLQ["HIGH"] &     self.sRLQ["HIGH"],      self.hT["NO"])
        
        self.handoverTriggerEvaluation_ctrl = ctrl.ControlSystem([htRule1, htRule2, htRule3, htRule4, 
                                                                    htRule5, htRule6, htRule7, htRule8, htRule9])
        self.handoverTriggerEvaluation = ctrl.ControlSystemSimulation(self.handoverTriggerEvaluation_ctrl)
        
    def evaluateServingCellRlq(self, rsrq, deltaRsrq):
        self.servingCellRlqEvaluation.input["RSRQ"] = rsrq
        self.servingCellRlqEvaluation.input["deltaRSRQ"] = deltaRsrq
        self.servingCellRlqEvaluation.compute()
        return self.servingCellRlqEvaluation.output["RLQ"]

    def evaluateNeighborCellRlq(self, rsrq, deltaRsrq):
        self.neighborCellRlqEvaluation.input["RSRQ"] = rsrq
        self.neighborCellRlqEvaluation.input["deltaRSRQ"] = deltaRsrq
        self.neighborCellRlqEvaluation.compute()
        return self.neighborCellRlqEvaluation.output["RLQ"]

    def evaluateHandoverTrigger (self, servingCellRlq, neighborCellRlq):
        self.handoverTriggerEvaluation.input["nRLQ"] = neighborCellRlq
        self.handoverTriggerEvaluation.input["sRLQ"] = servingCellRlq
        self.handoverTriggerEvaluation.compute()
        o = self.handoverTriggerEvaluation.output["hT"]
        # self.hT.view(sim=self.handoverTriggerEvaluation)
        print ('\nhT: ', '{:>6.4f}'.format(o), '\n')
        if o > 0.75:
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
        e = norm(x - xi)
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
    print (FH.evaluateServingCellRlq (20, +2))
    print (FH.evaluateNeighborCellRlq (21, -1))  
    print (FH.evaluateHandoverTrigger (0.7, 0.85))