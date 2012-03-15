<?php
require_once("vertretungsplan/globals.inc");

class vpApp {
  /** @var vpView */
  var $oViewToday;
  /** @var vpView */
  var $oViewTomorrow;
  /** @var constant */
  var $cMode;
  
  function vpApp(&$oParserToday, &$oParserTomorrow) {
    $this->oViewToday = new vpView($oParserToday);
    $this->oViewTomorrow = new vpView($oParserTomorrow);
  }

  /**
   * Set view mode: Pupil's or teacher's view
   * @param constant $cMode
   * @return void
   */
  function setMode($cMode) {
    $this->cMode = $cMode;
    $this->oViewToday->setMode($cMode);
    $this->oViewTomorrow->setMode($cMode);
  }
  
  /**
   * Print the document
   * @return void
   */
  function show() {
    $this->oViewToday->oParser->parse();
    $this->oViewTomorrow->oParser->parse();
    debugPrint("App: Views are: ".var_export($this->oViewToday,true).
      var_export($this->oViewTomorrow,true));
    
    $this->printOutput();
  }
  
  function printOutput() {
    /** TODO: change this */
    $g_autoReload = true;

    // HEADER
    $strPageTitle = "Vertretungsplan Online &ndash; ".
      ($this->cMode == VP_VIEW_MODE_TEACHER ? "Lehrer" : "Sch&uuml;ler");
    $strOut = "<?xml version='1.0'?>\n".
      "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n".
      "  'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>".
      "<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='de' lang='de'>".
      "<head>\n<title>$strPageTitle</title>\n".
      "<style type='text/css'>\n".
      "  body, td, th { font-family:Arial; /*font-size:105%;*/ cursor:crosshair; }\n".
      "  td, th { border:1px solid #808080; border-top-width:0; border-left-width:0; padding:0 .7em; }\n".
      "  td.tableheading { font-weight:bold; }\n".
      "  table { border:1px solid #808080; border-right-width:0; border-bottom-width:0; border-collapse:collapse; }\n".
      "  td { vertical-align:top; }\n".
      #"  .classeven { background-color: #d0d0ff; }\n".
      #"  .classodd { background-color: #ffffff; }\n".
      "  .dailymessages { margin-bottom:1em; border:4px double black; padding:1em; }\n".
      "  .noborder { border: 0px; }\n".
      "</style>";
    // Autoscroller
    if(!isset($_GET["noautoscroll"])) {
      $strOut .= "<script type='text/javascript'>\n".
        "b = 0;\n".
        "function bodyScroll()\n".
        "{\n".
        "    window.scrollTo(0, b);\n".
        "    b += 1;\n".
        "    if (b >= document.body.scrollHeight)\n";
      # on noautoreload, just cycle thru the page, otherwise, reload it
      if($g_autoReload) {
        $strOut .= "      window.setTimeout('location.reload()', 1)\n";
      } else {
        $strOut .= "      window.setTimeout('window.scrollTo(0,0)', 1)\n";
      }
      $strOut .= 
        "    else\n".
        "      window.setTimeout('bodyScroll()', 10);\n".
        "}\n".
        "</script>\n";
    } else {
      // Nur META-Refresh
      if($g_autoReload) {
        $strOut .= "<meta http-equiv='refresh' content='90' />\n";
      }
    }
    $strOut .= "</head>\n";
    if(!isset($_GET["noautoscroll"])) {
      $strOut .= "<body onload='window.scrollTo(0,0);setTimeout(\"bodyScroll()\", 5000)'>\n";
    } else {
      $strOut .= "<body onload='window.scrollTo(0,0);setTimeout(\"bodyScroll()\", 5000)'>\n";
    }
    echo $strOut;
    
    echo "<table style='margin-width:10px; border:0px;' width='100%'><tr>";
    echo "<td width='50%' class='noborder'>";
    $this->oViewToday->show();
    echo "</td><td class='noborder'>";
    $this->oViewTomorrow->show();
    echo "</td></tr></table>\n";
    
    echo "</body>\n</html>\n";
  }
}
?>
