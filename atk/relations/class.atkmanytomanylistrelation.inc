<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage relations
 *
 * @copyright (c) 2000-2007 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5798 $
 * $Id: class.atkmanytomanylistrelation.inc 5798 2008-03-05 13:41:26Z guido $
 */

userelation("atkmanytomanyrelation");

/**
 * Many-to-many list relation.
 *
 * The relation shows a list of available records in a selection list
 * from which multiple records can be selected.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage relations
 */
class atkManyToManyListRelation extends atkManyToManyRelation
{
  private $m_rows = 6;
  private $m_width = 200;
  
  /**
   * Get rows.
   * 
   * @return int rows
   */
  public function getRows()
  {
    return $this->m_rows;
  }
  
  /**
   * Set rows.
   * 
   * @param int rows
   */
  public function setRows($rows)
  {
    $this->m_rows = $rows;
  }
  
  /**
   * Get width (in pixels).
   * 
   * @return int width in pixels
   */
  public function getWidth()
  {
    return $this->m_width;
  }
  
  /**
   * Set (pixel) width.
   * 
   * @param int $width width in pixels
   */
  public function setWidth($width)
  {
    $this->m_width = $width;
  }
  
  /**
   * Return a piece of html code to edit the attribute.
   * 
   * @param $record current record
   * 
   * @return string piece of html code
   */
  public function edit($record="", $fieldprefix="", $mode="")
  {
    $this->createDestination();
    $this->createLink();
    
    $selected = $this->getSelectedRecords($record);
    $selectable = $this->_getSelectableRecords($record, $mode);

    if (count($selectable) == 0)
    {
      return $this->text('select_none');
    }

    $id = $this->getHtmlId($fieldprefix);
    $name = $fieldprefix.$this->fieldName();
    
    $result = '<select id="'.$id.'" name="'.$name.'[]['.$this->getRemoteKey().']" multiple="multiple" size="'.min(count($selectable), $this->getRows()).'" style="width: '.$this->getWidth().'px">';    
    
    foreach ($selectable as $row)
    {
      $key = $this->m_destInstance->primaryKey($row);
      $label = $this->m_destInstance->descriptor($row);
      $selectedStr = in_array($key, $selected) ? ' selected="selected"' : '';
      $value = $row[$this->m_destInstance->primaryKeyField()];
      
      $result.= '<option value="'.htmlentities($value).'"'.$selectedStr.'>'.$label.'</option>';
    }
    
    $result .= '</select>';
    
    return $result;
  }
}