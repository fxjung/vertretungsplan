<?php
require_once("vertretungsplan/globals.inc");
require_once("share.inc");
require_once("sec/secure.inc");

class vpIDeskView extends vpView {

  function printHeading() {
    // Heading
    Title(q(strftime("%A, %x", $this->oParser->oManager->tsCurDate)));
    echo sprintf("<p><i>Letzte &Auml;nderung: %s</i></p>", q(strftime("%A, %X", 
      $this->oParser->oManager->tsLastChange)));
  }
  
  function printStandInTableHeader() {
    debugPrint("printStandInTableHeader: mode is ".$this->cMode);
    if($this->cMode == VP_VIEW_MODE_TEACHER) {
      TreeView(array("Lehrer", "Stunde", "Klasse", "Raum", "Vertr. von", 
        "(Le.) nach", "Vertretungstext"));
    } elseif($this->cMode == VP_VIEW_MODE_PUPIL) {
      TreeView(array("Klasse", "Stunde", "Lehrer", "Vertr.", "Raum", 
      	"Vertr. von", "(Le.) nach", "Vertretungstext"));
    }
  }

#  function printStandInTableRow($oStandIn, $strTeacher = "", $strOldTeacher = "") {
#    if($this->cMode == VP_VIEW_MODE_TEACHER) {
#    } elseif($this->cMode == VP_VIEW_MODE_PUPIL) {
#      echo sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td>" .
#      	"<td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
#        q($vp->getClass()), q($vp->getLesson()),
#        q($vp->getOriginalTeacher()), q($vp->getStandInTeacher()),
#        q($vp->getRoom()), q($vp->getLessonFrom()), q($vp->getTeacherTo()),
#        q($vp->getText()));
#      }
#  }

  function printEmptyMessage() {
    TreeViewEmpty("Keine Vertretungen");
  }

  function printStandInTableFooter() {
    _TreeView();
  }
}
?>
