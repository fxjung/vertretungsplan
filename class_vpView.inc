<?php
require_once("vertretungsplan/globals.inc");

define("VP_VIEW_MODE_TEACHER", 0);
define("VP_VIEW_MODE_PUPIL", 1);

class vpView {
  /** @var vpParser */
  var $oParser;

  /** @var array of arrays of type vpStandIn */
  var $ararSortedStandIns;
  
  /** @var constant */
  var $cMode;
  
  /** @var string */
  var $strOut;
  
  /**
   * Constructor
   * @param vpManager $ovpManager
   * @return vpView
   */
  function vpView(&$oParser) {
    $this->oParser = &$oParser;
    $this->cMode = VP_VIEW_MODE_PUPIL;
  }

  /**
   * Set view mode: Pupil's or teacher's view
   * @param constant $cMode
   * @return void
   */
  function setMode($cMode) {
    debugPrint("vpView::setMode: setting mode ".$cMode);
    $this->cMode = $cMode;
  }

  function printHeading() {
    // Heading
    echo sprintf("<h2 style='clear:both'>%s&nbsp;&nbsp;&nbsp;".
      "<small>Letzte &Auml;nderung: %s</small></h2>\n", q(strftime("%A, %x",
      $this->oParser->oManager->tsCurDate)), q(strftime("%A, %X", 
      $this->oParser->oManager->tsLastChange)));
  }
  
  function printDailyMessages() {
    // Daily messages
    $strDailyMessages = nl2br(vpBeautifyString(q($this->oParser->oManager->strDailyMessages)));
    if($strDailyMessages != "") {
      echo "<div class='dailymessages'>";
      if(strtotime("Dec 24") < strtotime("monday",strtotime("tomorrow"))) {
        echo "<img style='float:right' src='candle.png'>\n";
      }
      echo $strDailyMessages;
      echo "</div>\n";
    }
  }

  function printAbsentTeachers() {
    // Absent teachers
    $strAbsentTeachers = vpBeautifyString(q($this->oParser->oManager->strAbsentTeachers));
    echo "<p>Abwesende Lehrer: " . q($strAbsentTeachers) . "</p>";
  }

  function printStandInTable() {
    // Stand in table
    $this->printStandInTableHeader();
    
    if(sizeof($this->oParser->oManager->getStandIns()) < 1) {
      // No stand ins today
      $this->printEmptyMessage();

    } else {
      if($this->cMode == VP_VIEW_MODE_TEACHER) {
        $this->sortByTeacher();
        #debugPrint("-------------\nprintStandInTable: ".print_r($this->ararSortedStandIns, true));
      
        $strOldTeacher = "";
        foreach($this->ararSortedStandIns as $strTeacher => $arTeacherStandIns) {
          foreach($arTeacherStandIns as $oStandIn) {
            if(!(preg_match("/[\w?]/", $strTeacher))) {
              continue;
            }
            $this->printStandInTableRow($oStandIn, $strTeacher, $strOldTeacher);
            $strOldTeacher = $strTeacher;
          }
        }
      } elseif($this->cMode == VP_VIEW_MODE_PUPIL) {
        $this->sortByClass();
        foreach($this->ararSortedStandIns as $arClassStandIns) {
          foreach($arClassStandIns as $oStandIn) {
            if(!(preg_match("/[\w]/", $oStandIn->getClass())) || 
              (strpos($oStandIn->getClass(), "(") !== false)) {
              continue;
            }
            $this->printStandInTableRow($oStandIn);
          }
        }
      }
    }
    echo "</table>\n";
  }
  
  function printStandInTableHeader() {
    if($this->cMode == VP_VIEW_MODE_TEACHER) {
      echo "<table width='100%'>\n<tr><th>Lehrer</th><th>Std.</th><th>Klasse</th>".
        "<th>Raum</th><th>(Lehrer)</th><th>Vertr.&nbsp;von&nbsp;&nbsp;&nbsp;</th><th>(Le.) nach</th>".
        "<th>Vertr.-Text</th></tr>\n";
    } elseif($this->cMode == VP_VIEW_MODE_PUPIL) {
      echo "<table width='100%'>\n<tr><th>Klasse</th><th>Std.</th><th>Lehrer</th>".
        "<th>Vertr.</th><th>Raum</th><th>Vertr.&nbsp;von&nbsp;&nbsp;&nbsp;</th><th>(Le.) ".
        "nach</th><th>Vertr.-Text</th></tr>\n";
    }
  }
  
  function printStandInTableRow($oStandIn, $strTeacher = "", $strOldTeacher = "") {
    if($this->cMode == VP_VIEW_MODE_TEACHER) {
      // Note: &nbsp; causes the cell to be shown (important for td border attribute)
      echo sprintf("<tr><td><b>%s</b>&nbsp;</td><td>%s&nbsp;</td><td>%s&nbsp;</td>".
        "<td>%s&nbsp;</td><td>%s&nbsp;</td><td>%s&nbsp;</td><td>%s&nbsp;</td><td>%s&nbsp;</td></tr>\n", 
        ($strTeacher == $strOldTeacher) ? "" : $strTeacher,
        q($oStandIn->getLesson()), q($oStandIn->getClass()),
        q($oStandIn->getRoom()), q($oStandIn->getOriginalTeacher()),
        q($oStandIn->getLessonFrom()), q($oStandIn->getTeacherTo()), 
        q($oStandIn->getText()));
    } elseif($this->cMode == VP_VIEW_MODE_PUPIL) {
      echo sprintf("<tr><td>%s&nbsp;</td><td>%s&nbsp;</td><td>%s&nbsp;</td><td>%s&nbsp;</td>".
        "<td>%s&nbsp;</td><td>%s&nbsp;</td><td>%s&nbsp;</td><td>%s&nbsp;</td></tr>\n", 
        q($oStandIn->getClass()), q($oStandIn->getLesson()), 
        q($oStandIn->getOriginalTeacher()), q($oStandIn->getStandInTeacher()),
        q($oStandIn->getRoom()), q($oStandIn->getLessonFrom()),
        q($oStandIn->getTeacherTo()), q($oStandIn->getText()));
    }
  }
  
  function printStandInTableFooter() {}
  
  function printEmptyMessage() {
    echo "<tr><td colspan='8' style='text-align:center; ".
      "font-style:italic'>Keine Vertretungen</td></tr>\n";
  }

  /**
   * Print the table with stand ins
   * @return void
   */
  function show() {
    // Abort if empty file
    if($this->oParser->oManager->tsCurDate < 1) {
      return;
    }
    
    // We want to process the output later
    ob_start();

    $this->printHeading();
    $this->printDailyMessages();
    $this->printAbsentTeachers();
    $this->printStandInTable();
    $this->printStandInTableFooter();

    $str = ob_get_clean();
    #debugPrint($str);
    echo vpBeautifyString($str);
  }
  
  /**
   * Sort stand ins by teacher
   * @return void
   */
  function sortByTeacher() {
    debugPrint("Sort by teacher");
    $this->ararSortedStandIns = array();
    $arStandIns = $this->oParser->oManager->getStandIns();
    foreach($arStandIns as $oStandIn) {
      // special case for Sondereinsatz: there is no original teacher
      static $sAnonTeacher;
      if(trim($oStandIn->getOriginalTeacher()) == "") {
        // increment by non-printables to get a unique key for the array and
        // at the same time not having anything confusing in the table
        $sAnonTeacher = $sAnonTeacher . " ";  
        $oStandIn->setOriginalTeacher($sAnonTeacher);
      }

      $strOT = $oStandIn->getOriginalTeacher();
      $strST = $oStandIn->getStandInTeacher();
      $nL = intval($oStandIn->getLesson());

      // original teacher can be set free ("Freisetzung"), in this case, do not
      // overwrite already existing stand in in this lesson
      unset($oOld);
      if(isset($this->ararSortedStandIns[$strOT]) and 
        isset($this->ararSortedStandIns[$strOT][$nL])) {
        $oOld = $this->ararSortedStandIns[$strOT][$nL];
      }
      // we can insert the stand in if one of the following cases is true:
      // - old stand in does not exist
      // - old one is not Freisetzung and new one is not Freisetzung
      if(!isset($oOld) or ($oOld->getTeacherTo() == "Freis." and $oStandIn->getTeacherTo() != "Freis.")) {
        if(isset($oOld)) {
          debugPrint("sortByTeacher(): overwriting existing stand in");
          debugPrint("old stand in was: ".var_export($oOld, true)."\n new stand in is: ".
            var_export($oStandIn, true));
        }
	debugPrint("Inserting stand in ".var_export($oStandIn, true));
        $this->ararSortedStandIns[$strST][$nL] = $oStandIn;
        // also notify the original teacher that he is not in duty
        if($strOT != $strST) {
          $oNewStandIn = clone $oStandIn; // PHP5 assigns objects by reference
          // field "Teacher to" can be like "Mi-27.8. / 4"
          if(trim($oStandIn->getTeacherTo()) == "") {
            $oNewStandIn->setTeacherTo("Entfall");
          }
          $this->ararSortedStandIns[$strOT][$nL] = $oNewStandIn;
        }

      } else {
        debugPrint("sortByTeacher(): not overwriting existing stand in with ".
          "Freisetzung");
        debugPrint("old stand in was: ".var_export($oOld, true)."\n new stand in was: ".
          var_export($oStandIn, true));
      }

      #debugPrint("############\nsortByTeacher: $strST: ".print_r($this->ararSortedStandIns[$strST], true));
      if(isset($this->ararSortedStandIns[$strST])) {
        ksort($this->ararSortedStandIns[$strST]);
      }
      if(isset($this->ararSortedStandIns[$strOT])) {
        ksort($this->ararSortedStandIns[$strOT]);
      }
    }
    ksort($this->ararSortedStandIns);

    debugPrint("sortByTeacher(): finished, array is now: ".var_export($this->ararSortedStandIns, true));
  }
  
  /**
   * Sort stand ins by class
   * @return void
   */
  function sortByClass() {
    debugPrint("Sort by class");	 
    $this->ararSortedStandIns = array();
    $arStandIns = $this->oParser->oManager->getStandIns();
    foreach($arStandIns as $oStandIn) {
      $strClass = $oStandIn->getClass();
      // Don't take the lessons as the indices, as there can be multiple
      // courses at the same time (e.g. elective courses)
      $this->ararSortedStandIns[$strClass][] = $oStandIn;
      usort($this->ararSortedStandIns[$strClass], "vpSortByLesson");
    }
    natSortKey($this->ararSortedStandIns);
  }
}


/**
 * Sort an array naturally by keys
 * @param array $arrIn
 * @return array
 */
function natSortKey(&$arrIn) {
  $key_array = array();
  $arrOut = array();
  foreach($arrIn as $key => $value) {
    $key_array[] = $key;
  }
  natsort($key_array);
  foreach($key_array as $key => $value) {
    $arrOut[$value] = $arrIn[$value];
  }
  $arrIn=$arrOut;
}

/**
 * Compare function for sorting vpStandIn objects per lesson
 * @param vpStandIn $oA
 * @param vpStandIn $oB
 * @return int
 */
function vpSortByLesson($oA, $oB) {
  if($oA->getLesson() == $oB->getLesson()) {
    return 0;
  } else {
    return ($oA->getLesson() < $oB->getLesson() ? -1 : 1);
  }
}
?>
