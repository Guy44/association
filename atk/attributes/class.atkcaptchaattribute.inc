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
   * @version $Revision: 6085 $
   * $Id: class.atkcaptchaattribute.inc 6085 2008-08-06 06:53:31Z ivo $
   * 
   */

  atkimport("atk.attributes.atkattribute");
  
  /**
   * With the atkCaptchaAttribute class you can easily add a captcha to a forum 
   * or guestbook.
   *
   * Use the flag AF_NOLABEL if you want to start at the beginning of the
   * line.
   *
   * @author Nico de Boer <nico@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkCaptchaAttribute extends atkAttribute 
  {

    /**
     * Constructor
     * @param $name   The name of the attribute
     * @param $flags  The attribute flags
     * @return -
     */
    function atkCaptchaAttribute($name, $flags=0) 
    {
    	// A Captcha attribute should not be searchable and sortable
      $flags |= AF_HIDE_SEARCH|AF_NO_SORT;

      $this->atkAttribute($name,$flags); // base class constructor
    }

    /**
     * Edit  record
     * Here it will only return the text, no edit box.
     * @param $record Array with fields
     * @param $fieldprefix 
     * @return Parsed string
     */
    function edit($record="", $fieldprefix="")
    {
      $target = "atk/ext/captcha/img/captcha.jpg.php";
      $img    = session_url("include.php?file=".$target);
      
      $html =  '<img src="'.$img.'"><br>';
      $html .= '<br>';
      $html .= '<small>'.atktext("captcha_explain", "atk").'</small><br>';
      $html .= '<input type="text" name="'.$fieldprefix.$this->fieldName().'">';
      return $html;
    }
    
    /**
     * Make sure the value is not stored. (always calculated on the fly)
     * @access private
     * @return int
     */
    function storageType() { return NOSTORE; }

    /**
     * Make sure the value is not loaded.
     * @access private
     * @return int
     */
    function loadType() { return NOLOAD; }

    function validate(&$record, $mode)
    {
      $sCaptchaCode = $record[$this->fieldName()];
      if (md5(strtoupper($sCaptchaCode)) != $_SESSION['php_captcha'])
      {
        triggerError($record, $this->fieldName(), 'error_captchafield');
      }

      // clear to prevent re-use
      $_SESSION['php_captcha'] = '';
    }
  }

?>
