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
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5876 $
   * $Id: class.atklistattribute.inc 5876 2008-04-11 18:22:35Z tjeerd $
   */

  atkimport("atk.attributes.atkattribute");

  /**
   * Do not translate the options
   */
  define("AF_NO_TRANSLATION", AF_SPECIFIC_1);

  /**
   * Do not add a default null option.
   */
  define("AF_LIST_NO_OBLIGATORY_NULL_ITEM", AF_SPECIFIC_2);

  /**
   * Do not add null option ever
   */
  define("AF_LIST_NO_NULL_ITEM", AF_SPECIFIC_3);

  /**
   * Add a default null option to obligatory items
   */
  define("AF_LIST_OBLIGATORY_NULL_ITEM", AF_SPECIFIC_4);


  /**
   * The atkListAttribute class represents an attribute of a node
   * that has a selectbox to select from predefined values.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkListAttribute extends atkAttribute
  {
    /**
     * Array with options for Listbox
     */
    var $m_options = array();

    /**
     * Array with values for Listbox
     */
    var $m_values = array();

    /**
     * Array for fast lookup of what value belongs to what option.
     */
    var $m_lookup = array();

    /**
     * Array which holds the options,values and lookup array in cache
     */
    var $m_types = array();

    /**
     * Attribute that is to be selected
     */
    var $m_selected;

    /**
     * Value that is used when list is empty, normally empty
     */
     var $m_emptyvalue;

     /**
      * The width of the dropdown list in pixels
      * @var int
      */
     var $m_width;

     var $m_onchangehandler_init = "newvalue = el.options[el.selectedIndex].value;\n";

    /**
     * Constructor.
     *
     * Warning: very old versions of this attribute supported passing the
     * parameters in a different order: $name, $flags, $optionArray.
     * This order used to be supported even when the new order was
     * implemented, but it has now been removed. Keep this in mind
     * when upgrading from a very old ATK version (pre ATK4).
     *
     * @param $name Name of the attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with values. If you don't use this parameter,
     *                    values are assumed to be the same as the options.
     * @param $flags Flags for this attribute
     * @param $size  Size of the attribute.
     */
    function atkListAttribute($name, $optionArray, $valueArray="", $flags=0, $size=0)
    {
      if (!is_array($valueArray) || count($valueArray)==0)
      {
        if (is_numeric($valueArray))
          $flags = $valueArray;
        $valueArray = $optionArray;
      }

      // If all values are numeric, we can use a numeric field to store the selected
      // value.
      $this->m_dbfieldtype = "number";
      for ($i=0, $_i=count($valueArray); $i<$_i && $this->m_dbfieldtype=="number"; $i++)
      {
        if (!is_numeric($valueArray[$i])) $this->m_dbfieldtype="string";
        // if one of the values is not a number, the fieldtype must be string, and
        // the loop is stopped.
      }

      // If no size is specified, the max size we have is equal to the biggest value.
      if ($size==0)
      {
        for ($i=0, $_i=count($valueArray); $i<$_i; $i++)
        {
          $size = max($size, strlen($valueArray[$i]));
        }
      }

      $this->atkAttribute($name, $flags, $size); // base class constructor

      $this->setOptions($optionArray, $valueArray);
    }

    /**
     * Creates a lookup array to speedup translations
     *
     * @param array $optionArray
     * @param array $valueArray
     */
    function createLookupArray($optionArray,$valueArray)
    {
      foreach ($optionArray AS $id => $option)
      {
        $this->m_lookup[$valueArray[$id]] = $option;
      }

      $this->_set("lookup",$this->m_lookup);
    }

    /**
     * get functions to access member variables.
     * For backwards compatibility we also check the old member variables m_options,
     * m_values and m_lookup.
     */
    function getOptions($rec=null) {
      if(!isset($this->m_types["options"]) || count($this->m_types["options"]) == 0)
        return $this->m_options;
      return $this->_get("options",$rec);
    }
    function getValues($rec=null) {
      if(!isset($this->m_types["values"]) || count($this->m_types["values"]) == 0)
        return $this->m_values;
      return $this->_get("values",$rec);
    }
    function getLookup($rec=null) {
      if(!isset($this->m_types["lookup"]) || count($this->m_types["lookup"]) == 0)
        return $this->m_lookup;
      return $this->_get("lookup",$rec);
    }

    /**
     * Returns one of the following arrays
     * options => optionarray
     * values => valuearray
     * lookup => lookuparray
     *
     * @param string $type
     */
    function _get($type,$rec=null)
    {
      return $this->m_types[$type];
    }

    function _set($type,$value)
    {
      $this->m_types[$type]=$value;
      return true;
    }

    /**
     * Display's text version of Record
     * @param $record
     * @return text string of $record
     */
    function display($record)
    {
      return $this->_translateValue($record[$this->fieldName()],$record);
    }

    /**
     * Translates the database value
     *
     * @param string $value
     * @return string
     */
    function _translateValue($value,$rec=null)
    {
      $lookup = $this->getLookup($rec);
      $res="";
      if(isset($lookup[$value]))
      {
        if($this->hasFlag(AF_NO_TRANSLATION)) $res = $lookup[$value];
        else $res = $this->text($lookup[$value]);
      }
      return $res;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="")
    {
      // todo: configurable rows
      $id = $this->getHtmlId($fieldprefix);
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_LEFTRIGHT);

      $onchange='';
      if (count($this->m_onchangecode))
      {
        $onchange = 'onChange="'.$id.'_onChange(this);"';
        $this->_renderChangeHandler($fieldprefix);
      }

      $result = '<select id="'.$id.'" name="'.$id.'"  class="atklistattribute" '.$onchange.($this->m_width?" style='width: {$this->m_width}px'":"").'>';

      $result.=$this->_addEmptyListOption();

      $values   = $this->getValues($record);
      $recvalue = atkArrayNvl($record,$this->fieldName());

      for ($i=0;$i<count($values);$i++)
      {
        // If the current value is selected or occurs in the record
        if ((!is_null($this->m_selected) && $values[$i] == $this->m_selected) ||
            ( is_null($this->m_selected) && $values[$i] == $recvalue && $recvalue!==""))
        {
          $sel = "selected";
        }
        else
        {
          $sel = "";
        }

        $result.= '<option value="'.$values[$i].'" '.$sel.'>'.$this->_translateValue($values[$i],$record);
      }

      $result.='</select>';
      return $result;
    }

    /**
     * If this attribute is NOT obligatory
     * Or if the attribute is obligatory and we set a config saying all obligatory lists should have a null item
     * and we didn't add the flag AF_LIST_NO_OBLIGATORY_NULL_ITEM
     * Or if the AF_LIST_OBLIGATORY_NULL_ITEM is set
     * ... we add an empty list option
     * @return The empty list option or an empty string
     */
    function _addEmptyListOption()
    {
      $ret='';

      // use a different (more descriptive) text for obligatory items
      $text_key = $this->hasFlag(AF_OBLIGATORY) ? "list_null_value_obligatory" : "list_null_value";

      if (!$this->hasFlag(AF_LIST_NO_NULL_ITEM) ||
           ($this->hasFlag(AF_OBLIGATORY) &&
           // CONFIG IS DEPRECATED
          ((atkconfig("list_obligatory_null_item")&&!$this->hasFlag(AF_LIST_NO_OBLIGATORY_NULL_ITEM))||
           ($this->hasFlag(AF_LIST_OBLIGATORY_NULL_ITEM)))))
      {
        $ret = '<option value="'.$this->m_emptyvalue.'">'.atktext($text_key, "", is_object($this->m_ownerInstance) ? $this->m_ownerInstance->m_type : "");
      }
      return $ret;
    }

    /**
     * Returns a piece of html code that can be used in a form to search values
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
      // todo: configurable rows

      $values = $this->getValues($record);
      $result = '<select ';
      if ($extended)
      {
        $result.='multiple size="'.min(5,count($values)+1).'"';
      }

      $result .= 'name="'.$this->getSearchFieldName($fieldprefix).'[]">';

      $result .= '<option value="">'.atktext('search_all');

      foreach ($values AS $value)
      {
        if (atk_in_array(((string)$value),$record[$this->fieldName()], true) && $record[$this->fieldName()]!=="")
        {
          $sel = "selected";
        }
        else
        {
          $sel = "";
        }

        $result.= '<option value="'.$value.'" '.$sel.'>'.$this->_translateValue($value,$record);
      }

      $result.='</select>';
      return $result;
    }

    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      // We only support 'exact' matches.
      // But you can select more than one value, which we search using the IN() statement,
      // which should work in any ansi compatible database.
      $searchcondition = "";
      if (is_array($value) && count($value)>0 && $value[0]!="") // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.
      {
        if (count($value)==1) // exactly one value
        {
          $searchcondition = $query->exactCondition($table.".".$this->fieldName(),$this->escapeSQL($value[0]));
        }
        else // search for more values using IN()
        {
          $searchcondition = $table.".".$this->fieldName()." IN ('".implode("','",$value)."')";
        }
      }
      return $searchcondition;
    }

    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("exact");
    }

    function dbFieldType()
    {
      // Fieldtype was determined in the constructor.
      return $this->m_dbfieldtype;
    }

    function setAutoHide($attrib, $valuearr)
    {
      $conditions = array();
      foreach ($valuearr as $value)
      {
        $conditions[] = "newvalue=='$value'";
      }
      $this->addOnChangeHandler("if (".implode('||', $conditions).") hideAttrib('$attrib'); else showAttrib('$attrib');");
    }

    /**
     * Sets the selected listitem
     *
     * @param string $selected the listitem you want to have selected
     *
     * @deprecated
     * @see atkNode::initial_values
     */
    function setSelected($selected)
    {
      $this->m_selected = $selected;
    }

    /**
     * Gets the selected listattribute
     *
     * @return string the selected listitem
     *
     * @deprecated
     * @see atkNode::initial_values
     */
    function getSelected()
    {
      return $this->m_selected;
    }

    /**
     * Sets the value for the empty entry in the list attribute
     * In normal cases you would just leave this empty, but certain cases
     * might demand you set a value.
     * @param string $value the value we set for empty value
     */
    function setEmptyValue($value)
    {
      $this->m_emptyvalue = $value;
    }

    /**
     * Gets the value for the empty entry in the list attribute
     * @return string
     */
    function getEmptyValue()
    {
      return $this->m_emptyvalue;
    }

    /**
     * Convert a String representation into an internal value.
     *
     * This implementation search for the value first in the valueArray, then in the optionArray
     * All other values are converted to the first of the valueArray
     *
     * @param String $stringvalue The value to parse.
     * @return Internal value (from valueArray)
     */
    function parseStringValue($stringvalue)
    {
      $values = $this->getValues();
      foreach($values as $value)
        if(strtolower($stringvalue) == strtolower($value))
          return $value;

      $i=0;
      $options = $this->getOptions();
      foreach($options as $option)
      {
        if(strtolower($stringvalue) == strtolower($option))
          return $values[$i];

        if(strtolower(atktext($stringvalue)) == strtolower($option))
          return $values[$i];
        $i++;
      }

      return $values[0];
    }

    /**
     * Set the width of the dropdown list in pixels
     * @param int $width The width of the dropdown list in pixels
     */
    function setWidth($width)
    {
      $this->m_width=$width;
    }

    /**
     * Gets the width of the dropdown list in pixels
     * @return int The width of the dropdown list in pixels
     */
    function getWidth()
    {
      return $this->m_width;
    }

    /**
     * Add option/value to dropdown
     *
     * @param string $option
     * @param string $value
     */
    function addOption($option, $value="")
    {
      if ($value!=0 && empty($value)) $value = $option;
      $currentOptions = $this->_get("options");
      $currentOptions[] = $option;
      $this->_set("options", $currentOptions);

      $currentValues = $this->_get("values");
      $currentValues[] = $value;
      $this->_set("values", $currentValues);

      $this->createLookupArray($currentOptions,$currentValues);
      return $this;
    }

    /**
     * Remove option from dropdown
     *
     * @param string $option
     */
    function removeOption($option)
    {
      $currentOptions = $this->_get("options");
      $currentValues = $this->_get("values");

      $index = array_search($option, $currentOptions);
      $value = $currentValues[$index];

      array_splice($currentOptions, $index, 1); // remove option
      array_splice($currentValues, $index, 1);  // remove value

      $this->_set("options",$currentOptions);
      $this->_set("values",$currentValues);
      return $this;
    }

    function setOptions($optionArray, $valueArray)
    {
      // m_options and m_values array are still here for backwardscompatibility
      $this->m_options = $optionArray;
      $this->_set("options",$optionArray);
      $this->m_values = $valueArray;
      $this->_set("values",$valueArray);

      $this->createLookupArray($optionArray,$valueArray);
      return $this;
    }
    
    /**
     * Remove value from dropdown
     *
     * @param string $value
     */
    function removeValue($value)
    {
      $currentOptions = $this->_get("options");
      $currentValues = $this->_get("values");

      $v = array_search($value, $currentValues);

      array_splice($currentOptions, $v, 1); // remove option
      array_splice($currentValues, $v, 1);  // remove value

      $this->_set("options",$currentOptions);
      $this->_set("values",$currentValues);
      return $this;
    }

  }

?>