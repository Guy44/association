<?php
 /**
  * This file is part of the Achievo ATK distribution.
  * Detailed copyright and licensing information can be found
  * in the doc/COPYRIGHT and doc/LICENSE files which should be
  * included in the distribution.
  *
  * @package atk
  * @subpackage handlers
  *
  * @copyright (c)2000-2004 Ivo Jansch
  * @license http://www.achievo.org/atk/licensing ATK Open Source License
  *
  * @version $Revision: 5901 $
  * $Id: class.atkselecthandler.inc 5901 2008-04-28 21:14:47Z yury $
  */

/**
 * Handler class for the select action of a node. The handler draws a
 * generic select form for searching through the records and selecting
 * one of the records.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage handlers
 */
class atkSelectHandler extends atkActionHandler
{
  /**
   * The action handler method.
   */
  public function action_select()
  {
    if (!empty($this->m_partial))
    {
      $this->partial($this->m_partial);
      return;
    }  
    
    $output = $this->invoke("selectPage");
    
    if ($output != "")
    {
      $this->getPage()->addContent($this->getNode()->renderActionPage("select", $output));
    }
  }

  /**
   * This method returns an html page containing a recordlist to select
   * records from. The recordlist can be searched, sorted etc. like an
   * admin screen.
   *
   * @return String The html select page.
   */
  public function selectPage()
  {
    $this->getNode()->addStyle("style.css");

    atkimport('atk.datagrid.atkdatagrid');
    $grid = atkDataGrid::create($this->getNode(), 'select');
    $actions = array('select' => atkurldecode($grid->getPostvar('atktarget')));
    $grid->setDefaultActions($actions);
    
    if ($this->autoSelectRecord($grid))
    {
      return '';
    }

    $params = array();
    $params["header"] = $this->getNode()->text("title_select");
    $params["list"] = $grid->render();
    $params["footer"] = "";
    
    if (atkLevel() > 0)
    {
      $backUrl = session_url(atkSelf().'?atklevel='.session_level(SESSION_BACK));
      $params["footer"] = '<br><div style="text-align: center"><input type="button" onclick="window.location=\''.$backUrl.'\';" value="&lt;&lt; '.$this->getNode()->text('back').'"></div>';
    }
    
    $output = $this->getUi()->renderList("select", $params);
    
    $vars = array("title" => $this->m_node->actionTitle('select'), "content" => $output);
    $output = $this->getUi()->renderBox($vars);    
    
    return $output;
  }
  
  /**
   * Update the admin datagrid.
   *
   * @return string new grid html
   */
  public function partial_datagrid()
  {
    atkimport('atk.datagrid.atkdatagrid');
    return atkDataGrid::resume($this->getNode())->render();
  }      

  /**
   * If the auto-select flag is set and only one record exists we immediately
   * return with the selected record.
   *
   * @param atkDataGrid $grid data grid
   * 
   * @return boolean auto-select active?
   */
  protected function autoSelectRecord($grid)
  {
    $node = $this->getNode();
    if (!$node->hasFlag(NF_AUTOSELECT))
    {
      return false;
    }
    
    $grid->loadRecords();
    if ($grid->getCount() != 1)
    {
      return false;
    }
    
    if (atkLevel() > 0 && $grid->getPostvar('atkprevlevel', 0) > atkLevel())
    {
      $backUrl = session_url(atkSelf().'?atklevel='.session_level(SESSION_BACK));
      $node->redirect($backUrl);
    }
    else
    {
      $records = $grid->getRecords();
      
      // There's only one record and the autoselect flag is set, so we
      // automatically go to the target.
      atkimport("atk.utils.atkstringparser");
      $parser = new atkStringParser(rawurldecode(atkurldecode($grid->getPostvar('atktarget'))));

      // For backwardscompatibility reasons, we also support the '[pk]' var.
      $records[0]['pk'] = $node->primaryKey($records[0]);
      $target = $parser->parse($records[0], true);

      $node->redirect(session_url($target, SESSION_NESTED));
    }
    
    return true;
  }
}