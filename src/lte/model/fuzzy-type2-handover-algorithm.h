/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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

#ifndef TYPE_2_FUZZY_HANDOVER_ALGORITHM_H
#define TYPE_2_FUZZY_HANDOVER_ALGORITHM_H

#include <ns3/lte-handover-algorithm.h>
#include <ns3/lte-handover-management-sap.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/simple-ref-count.h>
#include <ns3/ptr.h>
#include <map>

#include <queue>
#include <deque>

#ifdef _DEBUG
    #undef _DEBUG
    #include <python.h>
    #define _DEBUG
#else
    #include <Python.h>    
#endif

namespace ns3 {


/**
 * \brief Handover algorithm implementation based on RSRQ measurements, Event
 *        A2 and Event A4.
 *
 * Handover decision made by this algorithm is primarily based on Event A2
 * measurements (serving cell's RSRQ becomes worse than threshold). When the
 * event is triggered, the first condition of handover is fulfilled.
 *
 * Event A4 measurements (neighbour cell's RSRQ becomes better than threshold)
 * are used to detect neighbouring cells and their respective RSRQ. When a
 * neighbouring cell's RSRQ is higher than the serving cell's RSRQ by a certain
 * offset, then the second condition of handover is fulfilled.
 *
 * When the first and second conditions above are fulfilled, the algorithm
 * informs the eNodeB RRC to trigger a handover.
 *
 * The threshold for Event A2 can be configured in the `ServingCellThreshold`
 * attribute. The offset used in the second condition can also be configured by
 * setting the `NeighbourCellOffset` attribute.
 *
 * The following code snippet is an example of using and configuring the
 * handover algorithm in a simulation program:
 *
 *     Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
 *
 *     NodeContainer enbNodes;
 *     // configure the nodes here...
 *
 *     lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
 *     lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
 *                                               UintegerValue (30));
 *     lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
 *                                               UintegerValue (1));
 *     NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
 *
 * \note Setting the handover algorithm type and attributes after the call to
 *       LteHelper::InstallEnbDevice does not have any effect to the devices
 *       that have already been installed.
 */
class Type2FuzzyHandoverAlgorithm : public LteHandoverAlgorithm
{
public:
  /// Creates an type 2 fuzzy handover algorithm instance.
  Type2FuzzyHandoverAlgorithm ();

  virtual ~Type2FuzzyHandoverAlgorithm ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();

  // inherited from LteHandoverAlgorithm
  virtual void SetLteHandoverManagementSapUser (LteHandoverManagementSapUser* s);
  virtual LteHandoverManagementSapProvider* GetLteHandoverManagementSapProvider ();

  /// let the forwarder class access the protected and private members
  friend class MemberLteHandoverManagementSapProvider<Type2FuzzyHandoverAlgorithm>;

protected:
  // inherited from Object
  virtual void DoInitialize ();
  virtual void DoDispose ();

  // inherited from LteHandoverAlgorithm as a Handover Management SAP implementation
  void DoReportUeMeas (uint16_t rnti, LteRrcSap::MeasResults measResults);

private:
  /// The expected measurement identity for A2 measurements.
  uint8_t m_a2MeasId;
  /// The expected measurement identity for A4 measurements.
  uint8_t m_a4MeasId;

  // todo
  PyObject* pName, * pModule, * pDict, * pClass, * pInstance;

  /**
   * The `ServingCellThreshold` attribute. If the RSRQ of the serving cell is
   * worse than this threshold, neighbour cells are consider for handover.
   * Expressed in quantized range of [0..34] as per Section 9.1.7 of
   * 3GPP TS 36.133.
   */
  uint8_t m_servingCellThreshold;

  /**
   * The `NeighbourCellOffset` attribute. Minimum offset between the serving
   * and the best neighbour cell to trigger the handover. Expressed in
   * quantized range of [0..34] as per Section 9.1.7 of 3GPP TS 36.133.
   */
  uint8_t m_neighbourCellOffset;

  /// Interface to the eNodeB RRC instance.
  LteHandoverManagementSapUser* m_handoverManagementSapUser;
  /// Receive API calls from the eNodeB RRC instance.
  LteHandoverManagementSapProvider* m_handoverManagementSapProvider;

}; // end of class A2A4RsrqHandoverAlgorithm


} // end of namespace ns3


#endif /* A2_A4_RSRQ_HANDOVER_ALGORITHM_H */
