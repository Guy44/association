<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision:
   * $Id: class.atkmailer.inc 4478 2006-12-28 12:29:49Z patrick $
   */

  /**
   * @internal includes
   */
  include_once(atkconfig("atkroot").'atk/ext/phpmailer/class.phpmailer.php');

 /**
   * ATK mailer class
   *
   * This class can be used to send HTML e-mails.
   *
   * This is basically an extension of the PHPMailer class, to override some
   * basic settings.
   *
   * This class also supports a $config_mail_enabled config setting, which can
   * be set to false to disable all outgoing emails. (useful for test
   * environments that shouldn't actually send the mails)
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @version $Revision: 4478 $
   *
   * @package atk
   * @subpackage utils
   */
  class atkMailer extends PHPMailer
  {
    var $Mailer   = "mail";
    var $WordWrap = 75;

    /**
     * Override error handler.
     *
     * @param string $msg error message
     */
    function error_handler($msg)
    {
      atkerror($msg);
    }

    /**
     * Send.
     */
    function Send()
    {
      if (atkconfig("mail_enabled", true))
      {
        // make sure Sender is set so the Return-Path header will have a decent value
        if ($this->Sender == "") $this->Sender = $this->From;

        return parent::Send();
      }
      else return true;
    }

  }

?>
