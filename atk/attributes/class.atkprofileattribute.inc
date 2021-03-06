<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5980 $
   * $Id: class.atkprofileattribute.inc 5980 2008-06-26 12:44:27Z peter $
   */

  /**
   * The atkProfileAttribute is an attribute to edit a security profile.
   * The best way to use it is inside the class where you edit your
   * profile or usergroup records.
   *
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkProfileAttribute extends atkAttribute
  {
    var $m_parentAttrName;
    var $m_accessField;

    function atkProfileAttribute($name, $parentAttrName="", $flags=0)
    {
      if (is_numeric($parentAttrName))
      {
        $flags = $parentAttrName;
        $parentAttrName = "";
      }

      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH|AF_HIDE_LIST);
      $this->m_parentAttrName = $parentAttrName;
      
      $this->m_accessField = atkconfig('auth_accessfield');
      if (empty($this->m_accessField))
        $this->m_accessField = atkconfig('auth_levelfield');
    }

    function load(&$db, $record)
    {
      $query = "SELECT *
                FROM ".atkconfig("auth_accesstable")."
                WHERE ".$this->m_accessField."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";

      $result = Array();
      $rows = $db->getrows($query);
      for ($i=0;$i<count($rows);$i++)
      {
        $result[$rows[$i]["node"]][] = $rows[$i]["action"];
      }
      return $result;
    }

    function getChildGroups(&$db, $id)
    {
      $result = array();
      if (!is_numeric($id))
        return $result;

      $query = "SELECT ".$this->m_ownerInstance->primaryKeyField()." ".
               "FROM ".$this->m_ownerInstance->m_table." ".
               "WHERE ".$this->m_parentAttrName." = $id";


      $rows = $db->getRows($query);
      foreach ($rows as $row)
      {
        $id = $row[$this->m_ownerInstance->primaryKeyField()];
        $result = array_merge($result, array($id), $this->getChildGroups($db, $id));
      }

      return $result;
    }

    function store(&$db, $record, $mode)
    {
      global $g_user;

      // Read the current actions available/editable and user rights before changing them
      $isAdmin = ($g_user['name']=='administrator' || $this->canGrantAll());
      $allActions = $this->getAllActions($record);
      $editableActions = $this->getEditableActions($record);

      $delquery = "DELETE FROM ".atkconfig("auth_accesstable")."
                   WHERE ".$this->m_accessField."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";

      if ($db->query($delquery))
      {

        $checked = $record[$this->fieldName()];

        $children = array();
        if (!empty($this->m_parentAttrName))
          $children = $this->getChildGroups($db, $record[$this->m_ownerInstance->primaryKeyField()]);

        foreach ($checked as $node => $actions)
        {
          $actions = array_unique($actions);

          $nodeModule = getNodeModule($node);
          $nodeType = getNodeType($node);

          $validActions = array();

          if (is_array($allActions[$nodeModule][$nodeType]))
            $validActions = array_intersect($actions, $allActions[$nodeModule][$nodeType]);

          // If you're not an admin, leave out all actions which are not editable (none if no editable actions available)
          if (!$isAdmin)
            $validActions = isset($editableActions[$nodeModule][$nodeType]) ? array_intersect($validActions, $editableActions[$nodeModule][$nodeType]) : array();

          foreach ($validActions as $action)
          {
            $query = "INSERT INTO ".atkconfig("auth_accesstable")." (node, action, ".$this->m_accessField.") ";
            $query.= "VALUES ('".$db->escapeSQL($node)."','".$db->escapeSQL($action)."','".$record[$this->m_ownerInstance->primaryKeyField()]."')";

            if (!$db->query($query))
            {
              // error.
              return false;
            }
          }

          if (count($children) > 0 && count($validActions) > 0)
          {
            $query = "DELETE FROM ".atkconfig("auth_accesstable")." ".
                     "WHERE ".$this->m_accessField." IN (".implode(",", $children).") ".
                     "AND node = '".$db->escapeSQL($node)."' ".
                     "AND action NOT IN ('".implode("','", $validActions)."')";

            if (!$db->query($query))
            {
              // error.
              return false;
            }
          }
        }
      }

      return true;
    }

    function hide($record="", $fieldprefix="")
    {
      // get checks
      $checked = $record[$this->fieldName()];

      // rebuild hidden fields from checked boxes
      $rights  = "";

      foreach($checked as $key=>$val)
      {
        for ($i=0;$i<=count($val)-1;$i++)
        {
          $value   = $key.".".$val[$i];
          $rights .= '<input type="hidden" name="rights[]" value="'.$value.'">';
        }
      }
      return $rights;
    }

    /**
     * Retrieve all possible module/node actions.
     */
    function getAllActions($record)
    {
      global $g_modules, $g_moduleflags, $g_nodes;

      $result = array();

      // hierarchic groups, only return actions of parent (if this record has a parent)
      $parentAttr = $this->m_parentAttrName;
      if (!empty($parentAttr) && is_numeric($record[$parentAttr]))
      {
        $db = &atkGetDb();
        $query = "SELECT DISTINCT node, action FROM ".atkconfig("auth_accesstable")." ".
                 "WHERE ".$this->m_accessField." = ".$record[$parentAttr];
        $rows = $db->getRows($query);

        foreach ($rows as $row)
        {
          $module = getNodeModule($row['node']);
          $node = getNodeType($row['node']);
          $result[$module][$node][] = $row['action'];
        }
      }

      // non-hierarchic groups, or root
      else
      {
        // include node information
        require_once(atkconfig("atkroot")."atk/atknodetools.inc");
        if (file_exists("config.nodes.inc"))
          include_once("config.nodes.inc");

        // get nodes for each module
        foreach (array_keys($g_modules) as $module)
        {
          if (!isset($g_moduleflags[$module]) || !hasFlag($g_moduleflags[$module], MF_NORIGHTS))
          {
            $instance = &getModule($module);
            if (method_exists($instance, "getNodes"))
              $instance->getNodes();
          }
        }
        // retrieve all actions after we registered all actions
        foreach (array_keys($g_modules) as $module)
        {
          // retrieve actions for each node
          $nodes = isset($g_nodes[$module]) ? $g_nodes[$module] : array();
          foreach ($nodes as $node => $actions)
            if (is_array($actions))
              $result[$module][$node] = $actions;
        }
      }

      return $result;
    }

    /**
     * Returns a list of actions that should be edittable by the user.
     */
    function getEditableActions($record)
    {
      $user = getUser();
      $levels = "";
      if (!is_array($user['level']))
        $levels = "'".$user['level']."'";
      else $levels = "'".implode("','",$user['level'])."'";

      // retrieve editable actions by user's levels
      $db = &atkGetDb();
      $query = "SELECT DISTINCT node, action FROM ".atkconfig("auth_accesstable"). " WHERE ".$this->m_accessField." IN (".$levels.")";
      $rows = $db->getRows($query);

      $result = array();
      foreach ($rows as $row)
      {
        $module = getNodeModule($row['node']);
        $node = getNodeType($row['node']);
        $result[$module][$node][] = $row['action'];
      }

      return $result;
    }

    /**
     * Initially use an empty rights array.
     *
     * @return array initial rights
     */
    function initialValue()
    {
      return array();
    }
    
    /**
     * Returns the currently selected actions.
     */
    function getSelectedActions($record)
    {
      $selected = $record[$this->fieldName()];

      $result = array();
      foreach ($selected as $node => $actions)
      {
        $module = getNodeModule($node);
        $node = getNodeType($node);
        $result[$module][$node] = $actions;
      }

      return $result;
    }

    /**
     * Display rights.
     * It will only display the rights & nodes that are selected for the user.
     *
     * @param string $record
     *
     * @return string
     */
    public function display($record)
    {
      $user = getUser();
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkprofileattribute.js.php");
      $this->_restoreDivStates($page);

      $result  = '';
      $isAdmin =  ($user['name']=='administrator' || $this->canGrantAll());
      
      $allActions      = $this->getAllActions($record);
      $editableActions = $this->getEditableActions($record);
      $selectedActions = $this->getSelectedActions($record);

      $showModule = count($allActions) > 1 && ($isAdmin || count($editableActions) > 1);

      $firstModule = true;

      foreach ($allActions as $module => $nodes)
      {
        // If we have more then one module, split up the module results by collapsable div's
        if ($showModule)
        {
          $result .= "<br><hr>";
          if ($firstModule)
            $firstModule = false;
          else
            $result .= "</div><br>";
          $result .= "<b><a href=\"javascript:void(0)\" onclick=\"profile_swapProfileDiv('div_$module','".atkConfig("atkroot")."'); return false;\"><img src='".atkConfig("atkroot")."atk/images/plus.gif' border='0' id='img_div_$module'></a>&nbsp;</b>".atktext(array("title_$module", $module), $module)."<br>";
          $result .= "<div id='div_$module' name='div_$module' style='display: none;'>";
          $result .= "<input type='hidden' name=\"divstate['div_$module']\" id=\"divstate['div_$module']\" value='closed' />";
          $result .= "<br>";
        }
        
        foreach ($nodes as $node => $actions)
        {
          $showBox = $isAdmin || count(array_intersect($actions, (is_array($editableActions[$module][$node])?$editableActions[$module][$node]:array()))) > 0;
          $display_node_str       = false;
          $display_tabs_str       = false;
          $node_result            = '';
          $permissions_string     = '';
          $tab_permissions_string = '';

          foreach ($actions as $action)
          {
            $isSelected = isset($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node]);

            // If the action of a node is selected for this user we will show the node,
            // otherwise we won't
            if ($isSelected) 
            {
              $display_node_str = true;
              if (substr($action,0,4) == "tab_")
              {
                $display_tabs_str = true;
                $tab_permissions_string .= $this->permissionName($action, $node, $module) . '&nbsp;&nbsp;&nbsp;';
              }
              else
              {
                $permissions_string     .= $this->permissionName($action, $node, $module) . '&nbsp;&nbsp;&nbsp;';
              }
            }
          }
          
          if ($showBox)
          {
            $node_result .= "<b>".atktext($node, $module)."</b><br>";
            $node_result .=   $permissions_string;
            if ($display_tabs_str)
              $node_result .= "<br>Tabs:&nbsp;".$tab_permissions_string;
            $node_result .= "<br /><br />\n";
          }else{
            $node_result .= $permissions_string;
            if ($display_tabs_str)
              $node_result .= "<br>Tabs:&nbsp;".$tab_permissions_string;
          }
            
          if ($display_node_str)
            $result .= $node_result;
        }
      }

      return $result;
    }
    
    
    
    function edit($record)
    {
      $user = getUser();
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkprofileattribute.js.php");

      $this->_restoreDivStates($page);

      $result='<div align="right">
                  [<a href="javascript:void(0)" onclick="profile_checkAll(\''.$this->fieldName().'\'); return false;">'.
                    atktext("check_all").
             '</a> <a href="javascript:void(0)" onclick="profile_checkNone(\''.$this->fieldName().'\'); return false;">'.
                    atktext("check_none").
             '</a> <a href="javascript:void(0)" onclick="profile_checkInvert(\''.$this->fieldName().'\'); return false;">'.
                    atktext("invert_selection").'</a>]</div>';

      $isAdmin =  ($user['name']=='administrator' || $this->canGrantAll());
      $allActions = $this->getAllActions($record);
      $editableActions = $this->getEditableActions($record);
      $selectedActions = $this->getSelectedActions($record);

      $showModule = count($allActions) > 1 && ($isAdmin || count($editableActions) > 1);

      $firstModule = true;

      foreach ($allActions as $module => $nodes)
      {
        if ($showModule)
        {
          $result .= "<br><hr>";
          if ($firstModule)
            $firstModule = false;
          else
            $result .= "</div><br>";
          $result .= "<b><a href=\"javascript:void(0)\" onclick=\"profile_swapProfileDiv('div_$module','".atkConfig("atkroot")."'); return false;\"><img src='".atkConfig("atkroot")."atk/images/plus.gif' border='0' id='img_div_$module'></a>&nbsp;</b>".atktext(array("title_$module", $module), $module)."<br>";
          $result .= "<div id='div_$module' name='div_$module' style='display: none;'>";
          $result .= "<input type='hidden' name=\"divstate['div_$module']\" id=\"divstate['div_$module']\" value='closed' />";
          $result.='<div>
                  [<a href="javascript:void(0)" onclick="profile_checkAllByValue(\''.$this->fieldName().'\',\''.$module.'.\'); return false;">'.
                    atktext("check_all", "atk").
             '</a> <a href="javascript:void(0)" onclick="profile_checkNoneByValue(\''.$this->fieldName().'\',\''.$module.'.\'); return false;">'.
                    atktext("check_none", "atk").
             '</a> <a href="javascript:void(0)" onclick="profile_checkInvertByValue(\''.$this->fieldName().'\',\''.$module.'.\'); return false;">'.
                    atktext("invert_selection", "atk").'</a>]</div>';
          $result .= "<br>";
        }

        foreach ($nodes as $node => $actions)
        {
          $showBox = $isAdmin || count(array_intersect($actions, (is_array($editableActions[$module][$node])?$editableActions[$module][$node]:array()))) > 0;

          if ($showBox)
            $result .= "<b>".atktext($node, $module)."</b><br>";

          $tabs_str = "";
          $display_tabs_str = false;

          // Draw action checkboxes
          foreach ($actions as $action)
          {
            $temp_str = "";

            $isEditable = $isAdmin || atk_in_array($action, $editableActions[$module][$node]);
            $isSelected = isset($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node]);

            if ($isEditable)
            {
              if (substr($action,0,4) == "tab_")
                $display_tabs_str = true;

              $temp_str .= '<input type="checkbox" name="'.$this->formName().'[]" '.$this->getCSSClassAttribute("atkcheckbox").' value="'.$module.".".$node.".".$action.'" ';
              $temp_str .= ($isSelected ? ' checked="checked"' : '').'></input> ';
              $temp_str .= $this->permissionName($action, $node, $module).'&nbsp;&nbsp;&nbsp;';
            }

            if (substr($action,0,4) == "tab_")
              $tabs_str .= $temp_str;
            else
              $result .= $temp_str;
          }

          if ($display_tabs_str)
            $result .= "<br>Tabs:&nbsp;";

          $result .= $tabs_str;

          if ($showBox)
            $result .= "<br /><br />\n";
        }


      }

      return $result;
    }

    /**
     * Return the translated name of a permission.
     */
    function permissionName($action, $nodename="", $modulename="")
    {
      $keys = array(
        'permission_'.$modulename.'_'.$nodename.'_'.$action,
        'action_'.$modulename.'_'.$nodename.'_'.$action,
        'permission_'.$nodename.'_'.$action,
        'action_'.$nodename.'_'.$action,
        'permission_'.$action,
        'action_'.$action,
        $action
      );
      
      $label = $this->text($keys);
      return $label;
    }

    function fetchValue($postvars)
    {
      $checkboxes = array();
      if (isset($postvars[$this->fieldName()]))
      {
        $checkboxes = $postvars[$this->fieldName()];
      }
      $actions = Array();
      for($i=0;$i<count($checkboxes);$i++)
      {
        $elems = split("\.",$checkboxes[$i]);
        if (count($elems)==3)
        {
          $node = $elems[0].".".$elems[1];
          $action = $elems[2];
        }
        else if (count($elems)==2)
        {
          $node = $elems[0];
          $action = $elems[1];
        }
        else
        {
          // never happens..
          atkdebug("profileattribute encountered incomplete combination");
        }
        $actions[$node][] = $action;
      }
      return $actions;
    }

    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array();
    }

    function dbFieldType()
    {
      return "";
    }

    /**
     * Checks whether the current user has the 'grantall' privilege (if such a
     * privilege exists; this is determined by the application by setting
     * $config_auth_grantall_privilege.
     *
     * @return boolean
     */
    function canGrantAll()
    {
      $privilege_setting = atkconfig("auth_grantall_privilege");

      if ($privilege_setting!="")
      {
        global $g_securityManager;
        list($mod, $node, $priv) = explode(".", $privilege_setting);
        return $g_securityManager->allowed($mod.".".$node, $priv);
      }
      return false;
    }

    function _restoreDivStates(&$page)
    {
      $postvars = &$this->m_ownerInstance->m_postvars;
      if (!isset($postvars['divstate']) || !is_array($postvars['divstate']) || sizeof($postvars['divstate']) == 0)
        return;

      $divstate = $postvars['divstate'];
      $onLoadScript = "";

      foreach($divstate as $key => $value)
      {
        $key = substr($key,2,-2);
        if ($value == "opened")
          $onLoadScript .= "profile_swapProfileDiv('$key','".atkConfig("atkroot")."');";
      }
      $page->register_loadscript($onLoadScript);
    }
  }

?>
