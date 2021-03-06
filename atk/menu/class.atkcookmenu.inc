<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage menu
   *
   * @copyright (c)2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5228 $
   * $Id: class.atkcookmenu.inc 5228 2007-08-30 00:01:40Z ivo $
   */

  /**
   * Imports
   * @access private
   */
  atkimport("atk.menu.atkmodernmenu");

  /**
   * The atkBoolAttribute class represents an attribute of a node
   * that can either be true or false.
   *
   * @author Sandy Pleyte <sandy@achievo.org>
   * @package atk
   * @subpackage menu
   *
   */
  class atkcookmenu extends atkModernMenu
  {

    function getMenu()
    {
      global $g_menu;
      $atkmenutop="main";

      $menuitems = $this->getMenuItems($g_menu,$atkmenutop);
      $theme = &atkinstance("atk.ui.atktheme");
      $page = &atkinstance("atk.ui.atkpage");
      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("cookmenu.css"));
      $page->register_scriptcode("var myThemePanelBase='".str_replace('arrow.gif','',$theme->imgPath("arrow.gif"))."';",true);
      $page->register_script(atkconfig("atkroot")."atk/javascript/jscookmenu/JSCookMenu.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/jscookmenu/effect.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/jscookmenu/theme.js");
      $ui = &atkinstance("atk.ui.atkui");

       $menu_javascript= '<script language="JavaScript" type="text/javascript">
                          var atkCookMenu =
                          ['."\n";
       $this->getJavascriptMenuItems($menuitems,$menu_javascript);
       $menu_javascript.='];';


      $menu='</script>

      <div id="atkcookmenu"></div>
      <script language="JavaScript" type="text/javascript"><!--
      cmDraw (\'atkcookmenu\', atkCookMenu, \'vbr\', cmThemePanel, \'ThemePanel\');
      --></script>
      ';




        $box = $ui->renderBox(array("menu_javascript"=>$menu_javascript,"menu"=>$menu),"menu");
        return $box;

    }

    function getJavascriptMenuItems($menuitems,&$menu_javascript)
    {
      foreach($menuitems as $item)
      {
        if($item["name"]=="-")
        {
         $menu_javascript.="_cmSplit,\n";
        }
        else
        {
          if($item["url"]!="")
          {
            $url="'".$item["url"]."'";
            $target="'_self'";
          }
          else
          {
            $url="null";
            $target="null";
          }
          $menu_javascript.="[null, '".addslashes($item["name"])."', ".$url.", ".$target.", null";
          if(isset($item["submenu"]) && count($item["submenu"])>0)
          {
            $menu_javascript.=',';
            if($item["header"]!="")
            {
              $menu_javascript.="[_cmNoAction, '<td colspan=\"3\"".$item["header"]."</td>', null, null, null],\n";
              $menu_javascript.="_cmSplit,\n";

            }
            $this->getJavascriptMenuItems($item["submenu"],$menu_javascript);
          }
          $menu_javascript.="],\n";
        }
      }
    }

  }