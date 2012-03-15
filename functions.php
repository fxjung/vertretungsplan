<?php
function debugPrint($str) {
  if(isset($_GET["debug"])) {
    echo "<!-- $str -->\n";
  }
}

if(!function_exists("q")) {
  function q($str) { return htmlspecialchars($str); }
}

/**
 * Make strings some more beautiful
 * Remove multiple line feeds and control characters
 * @param string $str
 * @return string
 */
function vpBeautifyString($str) {
  $str = preg_replace("/[\x0d\x0c]/", "", $str);
  $str = preg_replace("/\\n\\n\\n*/", "\n\n", $str);
  return trim($str);
}

?>
