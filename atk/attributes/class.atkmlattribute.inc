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
   * @version $Revision: 5832 $
   * $Id: class.atkmlattribute.inc 5832 2008-03-28 15:33:50Z tjeerd $
   */

  /**
   * The atkMlAttribute class represents an multilanguage attribute of an atkNode.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkMlAttribute extends atkAttribute
  {
    var $m_language = "";
    var $m_mlattribute = TRUE;
    var $m_edited = false;
    var $m_languages = array();

    /**
     * Constructor
     * @param $name name of the attribute
     * @param $flags flags for the attribute
     */
    function atkMlAttribute($name, $flags = 0, $size = 0)
    {
      global $config_atkroot;
      /* base class constructor */
      //$this->atkAttribute($name, ($flags|AF_MULTILANGUAGE|(AF_PRIMARY^AF_OBLIGATORY))^(AF_PRIMARY^AF_OBLIGATORY), $size);
      $this->atkAttribute($name,$flags,$size);
      $this->m_language = strtoupper(atkconfig("defaultlanguage"));
    }

    function getValue($rec,$language="")
    {
      ($language=="")?$language=$this->m_language:$language=strtoupper($language);
      return $rec[$this->fieldName()][$language];
    }

    /**
    *
    */
    function isMlNode()
    {
      return ($this->m_ownerInstance->hasFlag(NF_ML));
    }

    /**
    * return possible languages
    */
    function getLanguages()
    {
      return atkconfig("supported_languages");
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value (one box which can be used for all the languages).
     * @param $record array with values
     * @return piece of html code
     */
    function edit($record="",$fieldprefix="")
    {
      $this->m_edited = true;
      /* register javascript */
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkmultilanguage.js.php");

      $languages = $this->getLanguages();
      $this->m_languages = $languages; // cache.

      /* hidden values */
      for ($i = 1; $i < sizeof($languages); $i++)
      {
        $result .= '<input type="hidden" name="'.$fieldprefix.$this->formName().'['.$languages[$i].']"'.
                   ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[$i]]).'" >';
      }

      /* default language */
      $result .= '<input type="text" name="'.$fieldprefix.$this->formName().'['.$languages[0].']"'.
                 ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[0]]).'"'.
                 ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                 ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';

      return $result;
    }

    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      parent::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);

      if ($this->m_edited) // if we edited this attrib, we also need to add the
      {
        $key= array_search($this->m_ownerInstance->m_postvars['atkeditlng'],$this->m_languages);
        $curlng = $this->m_languages[($key!==false?$key:1)];
        $entry = array("name" => $this->m_name."_ml", "obligatory" => $this->hasFlag(AF_OBLIGATORY), "attribute" => &$this);

        /* label? */
        $entry["label"] = $this->label($defaults).' (<label id="'.$fieldprefix.$this->formName().'_label">'.atktext("language_".strtolower($curlng)).'</label>)';
        $entry["id"] = $this->getHtmlId($fieldprefix);
        $entry["tabs"] = $this->m_tabs;
        $entry["sections"] = $this->m_sections;

        /* first language of other languages */
        $entry["html"] = '<input type="text" name="'.$fieldprefix.$this->formName().'['.(count($this->m_languages)==2 ? $curlng : "multilanguage").']"'.
                   ' value="'.htmlspecialchars($defaults[$this->fieldName()][$curlng]).'"'.
                   ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                   ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

        $arr["fields"][] = $entry;
        $this->m_edited = false;
      }
    }

    /**
     * Returns a piece of html code that can be used to search the attribute.
     * We will use the default language for this!
     * @param $record array with values
     * @return piece of htmlcode
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
      $languages = $this->getLanguages();

      $id  =  $this->getSearchFieldName($prefix);
      $id .= !$this->isMlNode()?'_'.$languages[0]:'';

      $result= '<input type="text" name="'.$id.'"'.
               ' value="'.$record[$this->fieldName().(!$this->isMlNode()?'_'.$languages[0]:'')].'"'.
               ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
               ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

      return $result;
    }

   /**
     * Check if the attribute is empty for all languages
     * @param array $record Current record
     * @return boolean true if it's empty
     */
    function isEmpty($record)
    {
      $languages = $this->getLanguages();
      for ($i=0,$_i=count($languages);$i<$_i;$i++)
      {
        if(isset($record[$this->fieldName()][$languages[$i]])) return 0;
      }

      return 1;
    }


    /**
     * Returns a displayable string for this value.
     * We always display the first language only.
     * @param $record array with values
     * @return string ready for displaying
     */
    function display($record)
    {
      if($this->isMlNode())
      {
        return nl2br(htmlspecialchars($record[$this->fieldName()]));
      }
      else
      {
        $languages = $this->getLanguages();
        return nl2br(htmlspecialchars($record[$this->fieldName()][$languages[0]]));
      }
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="")
    {
      $languages = $this->getLanguages();
      /* hidden values */
      for ($i = 0; $i < sizeof($languages); $i++)
      {
        $result .= '<input type="hidden" name="'.$this->formName().'['.$languages[$i].']"'.
                   ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[$i]]).'">';
      }
      return $result;
    }

    /**
    *
    */
    function value2db($rec)
    {
      if($this->isMlNode())
      {
        $rec[$this->fieldName()] = $rec[$this->fieldName()][$this->m_language];
      }
      return parent::value2db($rec);
    }

    /**
     * Removes slashes from the strings
     * @param $rec array with values
     * @return array with string without slashes
     */
    function db2value($rec)
    {
      $languages = $this->getLanguages();
      if($this->isMlNode())
      {
        if(is_array($rec[$this->fieldName()]))
        {
          foreach($rec[$this->fieldName()] as $lng=>$value)
          {
            $result[$lng]=stripslashes($value);
          }
        } else $result = stripslashes($rec[$this->fieldName()]);
      }
      else
      {
        for ($i=0,$_i=count($languages);$i<$_i;$i++)
        {
          $result[$languages[$i]] = stripslashes($rec[$this->fieldName().'_'.$languages[$i]]);
        }
      }
      return $result;
    }

    /**
     * Fetch values
     * @param $rec array with values
     * @return array with values
     */
    function fetchValue($rec)
    {
      if($this->isMlNode())
      {
        $res = parent::fetchValue($rec);
        return $res;
      }
      else
      {
        $languages = $this->getLanguages();
        for ($i=0,$_i=count($languages);$i<$_i;$i++)
        {
          $result[$languages[$i]] = stripslashes($rec[$this->fieldName()][$languages[$i]]);
        }
        return $result;
      }
    }

    /**
     * Add's fields to query
     * @param $query the SQL query
     * @param $tablename tablename
     * @param $fieldaliasprefix field alias prefix
     * @param $level ?
     * @param String $mode select/add/edit mode
     * @return new SQL query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec, $level, $mode)
    {
      $languages = $this->getLanguages();
      if(!$this->isMlNode())
      {
        if ($mode == "add" || $mode == "update")
        {
          for ($i=0,$_i=count($languages);$i<$_i;$i++)
          {
            $query->addField($this->fieldName().'_'.$languages[$i],$this->escapeSQL($rec[$this->fieldName()][$languages[$i]]),"","",!$this->hasFlag(AF_NO_QUOTES));
          }
        }
        else
        {
          for ($i=0,$_i=count($languages);$i<$_i;$i++)
          {
            $query->addField($this->fieldName().'_'.$languages[$i],"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
          }
        }
      }
      else
      {
        // when this node has multilanguage support we save multiple records per occurance
        // we only add the default language to this query
        parent::addToQuery($query,$tablename,$fieldaliasprefix,$rec,$level,$mode);
      }
    }

    /**
     * Fetch the metadata about this attrib from the table metadata
     */
    function fetchMeta($metadata)
    {
      $languages = $this->getLanguages();
      $attribname = $this->fieldName().(!$this->isMlNode()?'_'.$languages[0]:'');
      $this->m_size = min($metadata[$attribname]['len'], $this->maxInputSize());
      $this->m_searchsize = min($metadata[$attribname]['len'], 20);
      $this->m_maxsize = $metadata[$attribname]['len'];
    }

    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      $languages = $this->getLanguages();
      $func = $searchmode."Condition";
      if (method_exists($query,$func))
      {
        $searchcondition = $query->$func($table.".".$this->fieldName().(!$this->isMlNode()?'_'.$languages[0]:''),$this->escapeSQL($value));
      }
      else
      {
        atkdebug("Database doesn't support searchmode '$searchmode' for ".$this->fieldName().", ignoring condition.");
      }
      return $searchcondition;
    }
  }

?>
