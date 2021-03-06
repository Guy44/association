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
 * @copyright (c)2005 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 2955 $
 * $Id: class.atkeventlog.inc 2955 2005-12-01 21:35:24Z ivo $
 */

/**
 * Base class import.
 */
atkimport("atk.utils.atkactionlistener");
atkimport("atk.atktools");

/**
 * The atkEventLog is a ready to use atkActionListener for logging events
 * in a table.
 *
 * You can use the atkEventLog by adding an instance to a node using
 * atkNode's addListener() method.
 *
 * In order to use the atkEventLog, you have to have a table in the database
 * named 'atkeventlog' with the following structure:
 *
 * CREATE TABLE atkeventlog
 * (
 *   id INT(10),
 *   userid INT(10),
 *   stamp DATETIME,
 *   node VARCHAR(100),
 *   action VARCHAR(100),
 *   primarykey VARCHAR(255)
 * }
 *
 * The current implementation only supports the logging.
 * @todo Add visualisation of the log.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage utils
 */
class atkEventLog extends atkActionListener
{
	/**
	 * This method handles the storage of the action in the database.
	 *
	 * @param String $action The action being performed
	 * @param array $record The record on which the action is performed
	 */
	function actionPerformed($action, $record)
	{
		return true;
		$origine='atkeventlog.inc';
		$node = $this->m_node->atkNodeType();
		$selector=$this->m_node->primaryKey($record);
		$parametres=print_r($record,true);
		atk_log_event($origine, $node, $action, $parametres, $selector, $error_message, null);

		}}

	?>