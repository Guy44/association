<?php
  /* Setup the system */
//  $config_atkroot = "./";
//  require_once($config_atkroot."atk/class.atknode.inc");
  include_once("atk/atkbrowsertools.inc");
  if (is_object($g_sessionManager))
  {
    $g_theme["Name"] = $g_sessionManager->getValue("atkTheme", "globals");
  }
 
   
  if(empty($g_theme["Name"]) && ($g_user["name"]!="administrator"))
  { 
    $g_theme["Name"] = atkconfig("defaulttheme"); // default from configfile 
  
    $sql = "SELECT theme from app_utilisateur where identifiant='".$g_user["name"]."'";
    $db = &atkGetDb();
    $result = $db->getrows($sql);

    if(count($result)==1&&$result[0]["theme"]!="")
    {
      $g_theme["Name"] = $result[0]["theme"];    
    }
    if (is_object($g_sessionManager))
    {
      $g_sessionManager->globalVar("atkTheme", $g_theme["Name"], true);
    }
  }

  if(empty($g_theme["Name"]))
  {
    $g_theme["Name"] = atkconfig("defaulttheme"); // default from configfile
  }

  if (is_array(browserInfo::detectPDA())) $g_theme["Name"]="basic";
  
   if (is_object($g_sessionManager))
    {
      $g_sessionManager->globalVar("atkTheme", $g_theme["Name"], true);
    }
  
 //   print_r ($g_theme);
// die();
  
 /*
  * 
 $page = &atknew("atk.ui.atkpage");
  $ui = &atkinstance("atk.ui.atkui");
  $theme = &atkTheme::getInstance();

  $page->register_style($theme->stylePath("style.css"));
    
*/

atkimport("atk.layout");
  $layout = &layout::getInstance();

  $layout->initTheme();
  $layout->import_layout();

  
?>
