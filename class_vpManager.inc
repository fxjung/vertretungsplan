<?php
require_once("vertretungsplan/globals.inc");

class vpManager {

  /** @var array of type vpStandIn */
  var $aroStandIns;
  
  /** @var timestamp */
  var $tsLastChange;
  
  /** @var timestamp */
  var $tsCurDate;

  /** @var string */
  var $strDailyMessages;
  
  /** @var string */
  var $strAbsentTeachers;
  
  /** Constructor */
  function vpManager() {
    $this->tsCurDate = 0;
    $this->tsLastChange = 0;
    $this->strAbsentTeachers = "";
    $this->strDailyMessages = "";
    $this->flush();
  }

  /**
   * Insert a booking
   * @param vpStandIn 
   * @return void
   */
  function insert($ovpsi) {
    $this->aroStandIns = array_merge($this->aroStandIns, array($ovpsi));
    debugPrint("Manager: inserted stand in: ".var_export($ovpsi,true));
  }

  /**
   * Get the array of stand ins
   * @return array of type vpStandIn
   */
  function &getStandIns() {
    return $this->aroStandIns;
  }

  /** 
   * Delete all bookings 
   * @return void
   */
  function flush() {
    $this->aroStandIns = array();
  }
}
?>
