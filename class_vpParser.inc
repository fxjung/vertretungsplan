<?php
require_once("vertretungsplan/globals.inc");

// Note: some text-only printers have spaces at the beginning of the lines,
// probably due to a bad printer margin setting
define("VP_PATTERN_HEADER1", "/ *gp-Untis.*WILHELM-GYMN.*BRAUNSCHWEIG.*/");
define("VP_PATTERN_HEADER2", "/ *D-38102,.*LEONHARDSTR.\s*63/");
define("VP_PATTERN_DATELINE", "/^ *Online-Ausgabe.*\s(\d{1,2})\.(\d{1,2})\..*/");
define("VP_PATTERN_ABSENTTEACHERS", "/^ *Abwesende Lehrer:(.*)$/");
define("VP_PATTERN_MOREABSENTTEACHERS", "/^ *                 (.*)/");
define("VP_PATTERN_STANDINHEADER", "/^( *Klasse\(n\)\s*)(Stunde\s*)".
  "(\(Lehrer\)\s*)(Vertreter\s*)(Raum\s*)(Vertr\. von\s*)?(\(Le\.\) nach\s*)?".
  "(Vertretungs-Text)?/");

$g_arMonths = array(0,"January","February","March","April","May","June","July",
  "August","September","October","November","December");

class vpParser {

  /** @var vpManager */
  var $oManager;

  /** @var array of type string */
  var $arstrFileList;
  
  /** @var int */
  var $nCurLine;
  
  /** @var array of ints */
  var $arTableColumnWidths;
  
  /**
   * Constructor
   * @param vpManager
   * @return vpParser
   */
  function vpParser() {
    $this->oManager = new vpManager;
    $this->arstrFileList = array();
    $this->nCurLine = 0;
    $this->arTableColumnWidths = array();    
  }

  /////////////////////////////////////////////// BASIC OPERATIONS //////////////////////////////////////////////
  
  /**
   * Get vpManager object
   * @return vpManager
   */
  function &getManager() {
    return $this->oManager;
  }
  
  /**
   * Add a file to the list of files to read
   * @param string 
   * @return void
   */
  function addFile($strFileName) {
    $this->arstrFileList = array_merge($this->arstrFileList, array($strFileName));
  }

  /**
   * Delete all files from the list of files to read
   * @return void
   */
  function flushFiles() {
    $this->arstrFileList = array();
  }
  
  /**
   * Parses the files in the file list and inserts stand ins to the manager object
   * @return void
   * TODO: Something with the stand ins doesn't work yet 
   */
  function parse() {
    $arstrLines = array();
    // Read input files
    foreach($this->arstrFileList as $strFile) {
      // Save last modification time
      $this->oManager->tsLastChange = max($this->oManager->tsLastChange, filemtime($strFile));
      debugPrint("Read file $strFile, last changed on ".date("r",filemtime($strFile)));
      // Put lines into an array (and take care of form feeds :-/)
      $str = file_get_contents($strFile);
      #$str = iconv("ISO-8859-15", "UTF-8", $str);
      $str = preg_replace("/[\x0c]/", "\n", $str);
      $str = str_replace("\r", "", $str);
      $arstrLines = array_merge($arstrLines, split("\n", $str));
      debugPrint("Read lines: ".var_export($arstrLines,true));
    }
    
    // Process every single line
    $bReadDailyMessages = false;
    $bReadStandIns = false;
    $strLine = "";

    for($this->nCurLine = 0; $this->nCurLine < count($arstrLines) &&
      isset($arstrLines[$this->nCurLine+1]); $this->nCurLine++, $strLine = 
      $arstrLines[$this->nCurLine]) {

      // Line of daily messages
      if($bReadDailyMessages) {
        $this->oManager->strDailyMessages .= $strLine . "\n";
        // Stop when absent teachers begin 
        if($this->isAbsentTeachersLine($arstrLines[$this->nCurLine+1]) or
          $this->isStandInHeaderLine($arstrLines[$this->nCurLine+1])) {
          debugPrint("No more daily messages");
          $bReadDailyMessages = false;
        }
        
      // Line of stand in table
      } elseif($bReadStandIns) {
        $this->processStandInLine($strLine);
        if(!isset($arstrLines[$this->nCurLine+1]) or
	  $this->isHeaderLine($arstrLines[$this->nCurLine+1])) {
          debugPrint("No more stand ins");
          $bReadStandIns = false;
        }
      
      // Header line
      } elseif($this->isHeaderLine($strLine)) {
        $this->processHeaderLine($strLine);
        
      // Date line, and after that read daily messages
      } elseif($this->isDateLine($strLine)) {
        $this->processDateLine($strLine);
        debugPrint("Start to read daily messages");
        $bReadDailyMessages = true;
        
      // Absent teachers
      } elseif($this->isAbsentTeachersLine($strLine)) {
        $this->processAbsentTeachersLine($strLine);
      
      // Stand in table header line
      } elseif($this->isStandInHeaderLine($strLine)) {
        $this->processStandInHeaderLine($strLine);
        debugPrint("Start to read stand ins");
        $bReadStandIns = true;
      
      // Rubbish
      } else {
        debugPrint(sprintf("Line %d is rubbish", $this->nCurLine));
#        $this->nCurLine++;
      }
    }
  }
  
  ////////////////////////////////////// FUNCTIONS FOR LINE CATEGORISATION //////////////////////////////////////
  
  /**
   * Determine if a line is a header line
   * @param string
   * @return bool
   */
  function isHeaderLine($strLine) {
    if((preg_match(VP_PATTERN_HEADER1, $strLine) != 0) ||
      (preg_match(VP_PATTERN_HEADER2, $strLine) != 0)) {
      debugPrint(sprintf("Header at line %d: %s", $this->nCurLine, $strLine));
      return true;
    } else {
      return false;
    }
  }

  /**
   * Determine if a line is a date line
   * @param string
   * @return bool
   */
  function isDateLine($strLine) {
    if(preg_match(VP_PATTERN_DATELINE, $strLine) != 0) {
      debugPrint(sprintf("Date at line %d: %s", $this->nCurLine, $strLine));
      return true;
    } else {
      return false;
    }
  }

  /**
   * Determine if a line contains absent teachers
   * @param string
   * @return bool
   */
  function isAbsentTeachersLine($strLine) {
    if((preg_match(VP_PATTERN_ABSENTTEACHERS, $strLine) != 0) ||
      (preg_match(VP_PATTERN_MOREABSENTTEACHERS, $strLine) != 0)) {
      debugPrint(sprintf("Absent teachers at line %d: %s", $this->nCurLine, $strLine));
      return true;
    } else {
      return false;
    }
  }
 
  /**
   * Determine if a line contains the header for the stand in table
   * @param string
   * @return bool
   */
  function isStandInHeaderLine($strLine) {
    if((preg_match(VP_PATTERN_STANDINHEADER, $strLine) != 0)) {
      debugPrint(sprintf("Stand in table header at line %d: %s", $this->nCurLine, $strLine));
      return true;
    } else {
      return false;
    }
  }
  
  //////////////////////////////////////// FUNCTIONS FOR LINE PROCESSING ////////////////////////////////////////

  /**
   * Process the header line(s)
   * @param string
   * @return void
   */
  function processHeaderLine($strLine) { /**$this->nCurLine++;*/ }
  
  /**
   * Process the date line and saves the current date to $tsCurDate
   * @param string
   * @return void
   */
  function processDateLine($strLine) {
    global $g_arMonths;
    $arMatches = array();
    preg_match(VP_PATTERN_DATELINE, $strLine, $arMatches);
    $this->oManager->tsCurDate = strtotime($g_arMonths[$arMatches[2]]." ".$arMatches[1]);
    #$this->nCurLine++;
  }

  /**
   * Process the line(s) with absent teachers and fill $strAbsentTeachers
   * @param string
   * @return void
   */
  function processAbsentTeachersLine($strLine) {
    $arMatches = array();
    if(preg_match(VP_PATTERN_ABSENTTEACHERS, $strLine, $arMatches)) {
      // Read absent teachers
      $this->oManager->strAbsentTeachers .= trim($arMatches[1]);
      debugPrint("Absent teachers: ".$arMatches[1]);
    } elseif(preg_match(VP_PATTERN_MOREABSENTTEACHERS, $strLine, $arMatches)) {
      // Continue to read absent teachers in the next line
			if(substr(trim($this->oManager->strAbsentTeachers), -1, 1) != ",") {
				$this->oManager->strAbsentTeachers .= ", ";
			}
      $this->oManager->strAbsentTeachers .= trim($arMatches[1]);
      debugPrint("More absent teachers: ".$arMatches[1]);
    }
  }
  
  /**
   * Process the header of the stand in table, calculate the column withds
   * @param string
   * @return void
   */
  function processStandInHeaderLine($strLine) {
    // Set column widths
    $arMatches = array();
    preg_match(VP_PATTERN_STANDINHEADER, $strLine, $arMatches);
    for($n = 0; $n < 8; $n++) {
      $this->arTableColumnWidths[$n] = strpos($arMatches[0], $arMatches[$n+1])+1;
    }
    debugPrint("Stand in header columns are: ".var_export($this->arTableColumnWidths, true));
  }
  
  /**
   * Process a line from the stand in table and insert it into the manager object
   * @param string
   */
  function processStandInLine($strLine) {
    if(trim($strLine == "")) {
      return;     // Don't process empty lines
    }
    $arMatches = array();
    $len = $this->arTableColumnWidths;

    $strClass = trim(substr($strLine, 0, $len[1]-1));
    // WORKAROUND
    // sometimes when class is empty, the line is indented by one space
    // this disrupts our nice table layout... :-/
    if($strClass == "") {
      $strLine = substr($strLine, 1, strlen($strLine));
    }
    $strLesson = trim(substr($strLine, $len[1]-1, $len[2]-$len[1]-1));
    $strOriginalTeacher = trim(substr($strLine, $len[2]-1, $len[3]-$len[2]-1));
    $strStandInTeacher = trim(substr($strLine, $len[3]-1, $len[4]-$len[3]-1));
    $strRoom = trim(substr($strLine, $len[4]-1, $len[5]-$len[4]-1));
    $strLessonFrom = ($len[5]>$len[4]) ? trim(substr($strLine, $len[5]-1, $len[6]-$len[5]-1)) : "";
    $strTeacherTo = ($len[6]>$len[5]) ? trim(substr($strLine, $len[6]-1, $len[7]-$len[6]-1)) : "";
    $strText = ($len[7]>$len[6]) ? trim(substr($strLine, $len[7]-1)) : "";
    #$strText = trim(substr($strLine, $len[7]-1));

    debugPrint(sprintf("Lesson %d in class %s, original teacher: %s, new teacher: %s, room: %s,".
      "text: %s", $strLesson, $strClass, $strOriginalTeacher, $strStandInTeacher,
      $strRoom, $strText));

    // TODO
    // Parse class like "12gmsn13gms" or "11abcd"
//    preg_match("/^(\d{1,2})(a|b|c|d|g|ms|n){1-4}*/", $strClass, $arMatches);
//    for($i = 0; $i < strlen($arMatches[2]); $i++) {
      // Insert into the manager object
      $vpsi = new vpStandIn;
      $vpsi->setClass($strClass);
      $vpsi->setDate($this->oManager->tsCurDate);
      $vpsi->setLesson($strLesson);
      $vpsi->setOriginalTeacher($strOriginalTeacher);
      $vpsi->setStandInTeacher($strStandInTeacher);
      $vpsi->setRoom($strRoom);
      $vpsi->setLessonFrom($strLessonFrom);
      $vpsi->setTeacherTo($strTeacherTo);
      $vpsi->setText($strText);
      $this->oManager->insert($vpsi);
//    }
  }
}
?>
