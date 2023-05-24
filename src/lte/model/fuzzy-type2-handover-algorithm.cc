/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 * Copyright (c) 2020 Amiraslan Haghrah
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Original work authors (from lte-enb-rrc.cc):
 * - Nicola Baldo <nbaldo@cttc.es>
 * - Marco Miozzo <mmiozzo@cttc.es>
 * - Manuel Requena <manuel.requena@cttc.es>
 *
 * Converted to type 2 fuzzy handover algorithm interface by:
 * - Amiraslan Haghrah <amiraslanhaghrah@gmail.com>
 */

#include "fuzzy-type2-handover-algorithm.h"
#include <ns3/log.h>
#include <ns3/uinteger.h>

// #include <ns3/random-variable-stream.h>
#include "ns3/core-module.h"


#include <iomanip>
#include <numeric>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Type2FuzzyHandoverAlgorithm");

NS_OBJECT_ENSURE_REGISTERED (Type2FuzzyHandoverAlgorithm);


///////////////////////////////////////////
// Handover Management SAP forwarder
///////////////////////////////////////////


Type2FuzzyHandoverAlgorithm::Type2FuzzyHandoverAlgorithm ()
  : m_a2MeasId (0),
    m_a4MeasId (0),
    m_servingCellThreshold (30),
    m_neighbourCellOffset (1),
    m_handoverManagementSapUser (0)
{
  NS_LOG_FUNCTION (this);
  m_handoverManagementSapProvider = new MemberLteHandoverManagementSapProvider<Type2FuzzyHandoverAlgorithm> (this);
}


Type2FuzzyHandoverAlgorithm::~Type2FuzzyHandoverAlgorithm ()
{
  // Clean up
  Py_DECREF(pName);
  Py_Finalize();
  
  NS_LOG_FUNCTION (this);
}


TypeId
Type2FuzzyHandoverAlgorithm::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Type2FuzzyHandoverAlgorithm")
    .SetParent<LteHandoverAlgorithm> ()
    .SetGroupName("Lte")
    .AddConstructor<Type2FuzzyHandoverAlgorithm> ()
    .AddAttribute ("ServingCellThreshold",
                   "If the RSRQ of the serving cell is worse than this "
                   "threshold, neighbour cells are consider for handover. "
                   "Expressed in quantized range of [0..34] as per Section "
                   "9.1.7 of 3GPP TS 36.133.",
                   UintegerValue (34),
                   MakeUintegerAccessor (&Type2FuzzyHandoverAlgorithm::m_servingCellThreshold),
                   MakeUintegerChecker<uint8_t> (0, 34))
    .AddAttribute ("NeighbourCellOffset",
                   "Minimum offset between the serving and the best neighbour "
                   "cell to trigger the handover. Expressed in quantized "
                   "range of [0..34] as per Section 9.1.7 of 3GPP TS 36.133.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&Type2FuzzyHandoverAlgorithm::m_neighbourCellOffset),
                   MakeUintegerChecker<uint8_t> ())
  ;
  return tid;
}


void
Type2FuzzyHandoverAlgorithm::SetLteHandoverManagementSapUser (LteHandoverManagementSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_handoverManagementSapUser = s;
}


LteHandoverManagementSapProvider*
Type2FuzzyHandoverAlgorithm::GetLteHandoverManagementSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_handoverManagementSapProvider;
}


void
Type2FuzzyHandoverAlgorithm::DoInitialize ()
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (this << " requesting Event A2 measurements"
                     << " (threshold=" << (uint16_t) m_servingCellThreshold << ")");
  LteRrcSap::ReportConfigEutra reportConfigA2;
  reportConfigA2.eventId = LteRrcSap::ReportConfigEutra::EVENT_A2;
  reportConfigA2.threshold1.choice = LteRrcSap::ThresholdEutra::THRESHOLD_RSRQ;
  reportConfigA2.threshold1.range = m_servingCellThreshold;
  reportConfigA2.triggerQuantity = LteRrcSap::ReportConfigEutra::RSRQ;
  reportConfigA2.reportInterval = LteRrcSap::ReportConfigEutra::MS120;
  m_a2MeasId = m_handoverManagementSapUser->AddUeMeasReportConfigForHandover (reportConfigA2);

  NS_LOG_LOGIC (this << " requesting Event A4 measurements"
                     << " (threshold=0)");
  LteRrcSap::ReportConfigEutra reportConfigA4;
  reportConfigA4.eventId = LteRrcSap::ReportConfigEutra::EVENT_A4;
  reportConfigA4.threshold1.choice = LteRrcSap::ThresholdEutra::THRESHOLD_RSRQ;
  reportConfigA4.threshold1.range = 0; // intentionally very low threshold
  reportConfigA4.triggerQuantity = LteRrcSap::ReportConfigEutra::RSRQ;
  reportConfigA4.reportInterval = LteRrcSap::ReportConfigEutra::MS120;
  m_a4MeasId = m_handoverManagementSapUser->AddUeMeasReportConfigForHandover (reportConfigA4);

  setenv("PYTHONPATH", "/mnt/data/Ph.D./ns-3/ns-3-dev/src/lte/model/fuzzy/", 1);

  Py_Initialize();

  pName = PyUnicode_FromString("type2fuzzy");
  pModule = PyImport_Import(pName);
  pDict = PyModule_GetDict(pModule);

  // Build the name of a callable class 
  pClass = PyDict_GetItemString(pDict, "FuzzyHandover");

  // Create an instance of the class
  if (PyCallable_Check(pClass)) {
      pInstance = PyObject_CallObject(pClass, NULL);
  }

  LteHandoverAlgorithm::DoInitialize ();
}


void
Type2FuzzyHandoverAlgorithm::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  // Clean up
  Py_DECREF(pName);
  Py_Finalize();

  delete m_handoverManagementSapProvider;
}


void
Type2FuzzyHandoverAlgorithm::DoReportUeMeas  (uint16_t rnti,
                                              LteRrcSap::MeasResults measResults)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) measResults.measId);

  if (measResults.measId == m_a2MeasId)
    {
      uint16_t bestNeighbourCellId = _PyLong_AsInt ( PyObject_CallMethod ( pInstance, "EvaluateHandover", "(diii)", 
                                                                          Simulator::Now ().GetSeconds(), 
                                                                          rnti, 
                                                                          measResults.rsrpResult, 
                                                                          measResults.rsrqResult )); 

      // Trigger Handover, if needed
      if (bestNeighbourCellId > 0)
        {
          NS_LOG_LOGIC ("Best neighbour cellId " << bestNeighbourCellId);

          NS_LOG_LOGIC ("Trigger Handover to cellId " << bestNeighbourCellId);;

          // Inform eNodeB RRC about handover
          m_handoverManagementSapUser->TriggerHandover (rnti, bestNeighbourCellId);            
        }
    }
  else if (measResults.measId == m_a4MeasId)
    {
      if (measResults.haveMeasResultNeighCells
          && !measResults.measResultListEutra.empty ())
        {          
          for (std::list <LteRrcSap::MeasResultEutra>::iterator it = measResults.measResultListEutra.begin ();
               it != measResults.measResultListEutra.end ();
               ++it)
            {
              NS_ASSERT_MSG (it->haveRsrpResult == true,
                             "RSRP measurement is missing from cellId " << it->physCellId);
              NS_ASSERT_MSG (it->haveRsrqResult == true,
                             "RSRQ measurement is missing from cellId " << it->physCellId);

              PyObject_CallMethod ( pInstance, "UpdateNeighbourMeasurements", "(diiii)", 
                                    Simulator::Now ().GetSeconds(), 
                                    rnti, 
                                    it->physCellId, 
                                    it->rsrpResult, 
                                    it->rsrqResult);         
            }          
        }
      else
        {
          NS_LOG_WARN (this << " Event A4 received without measurement results from neighbouring cells");
        }
    }
  else
    {
      NS_LOG_WARN ("Ignoring measId " << (uint16_t) measResults.measId);
    }

} // end of DoReportUeMeas

} // end of namespace ns3
