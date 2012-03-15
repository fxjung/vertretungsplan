<?php
require_once("share.inc");
require_once("ctrl.inc");
require_once("vertretungsplan/class_vpApp.inc");
require_once("vertretungsplan/class_vpIDeskView.inc");

// default output is UTF-8, but the printouts of the stand in table is ISO-8859
header("Content-Type: text/html;charset=iso-8859-1");

class vpIDeskApp extends vpApp {
  function vpIDeskApp(&$oParserToday, &$oParserTomorrow) {
    $this->oViewToday = new vpIDeskView($oParserToday);
    $this->oViewTomorrow = new vpIDeskView($oParserTomorrow);
    
    // Automagically set pupil or teacher mode
    global $user;
    if(isset($_SESSION["groups"]) && isset($_SESSION["groups"]["lehrer"])) {
      $this->setMode(VP_VIEW_MODE_TEACHER);
      debugPrint("vpIDeskApp: Setting teacher mode");
    } else {
      $this->setMode(VP_VIEW_MODE_PUPIL);
      debugPrint("vpIDeskApp: Setting pupil mode");
    }
  }

  function printOutput() {
    html_header("<style type='text/css'>
    .treeview td { border-bottom: 1px solid #ccc; padding-bottom: 1px}
    </style>\n");
    
    PageBlue("Vertretungsplan", "vertretungsplan");
    #page_open();
    Option("Heute", "Vertretungsplan für heute", "href='#today'");
    Option("Morgen", "Vertretungsplan für morgen", "href='#tomorrow'");
    echo "<a name='today' id='today'></a>\n";
    $this->oViewToday->show();
    echo "<p /><a name='tomorrow' id='tomorrow'></a>\n";
    $this->oViewTomorrow->show();
    echo "<p></p>";
    _PageBlue();
    #page_close();
  }
}
?>
