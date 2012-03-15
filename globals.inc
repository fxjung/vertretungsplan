<?php
error_reporting(E_ALL & ~E_NOTICE);

setlocale(LC_ALL, "de_DE.UTF-8", "de_DE", "de");

require_once("class_vpStandIn.inc");
require_once("class_vpManager.inc");
require_once("class_vpParser.inc");
require_once("class_vpView.inc");
require_once("class_vpApp.inc");
require_once("functions.php");

$g_oParserToday = new vpParser;
$g_oParserTomorrow = new vpParser;
$g_oParserTomorrow->addFile("/group/vertretungsplan/Homepage/Morgen.prn");
$g_oParserToday->addFile("/group/vertretungsplan/Homepage/Heute.prn");

$g_oViewToday = new vpView($g_oParserToday);
$g_oViewTomorrow = new vpView($g_oParserTomorrow);

$g_oApp = new vpApp($g_oParserToday, $g_oParserTomorrow);
?>
