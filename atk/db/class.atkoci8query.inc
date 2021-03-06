<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6074 $
   * $Id: class.atkoci8query.inc 6074 2008-07-31 09:28:02Z peter $
   */

  /**
   * Base include
   */
  atkimport("atk.db.atkquery");

 /**
   * Query build for Oracle 8i databases.
   *
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */
  class atkOci8Query extends atkQuery
  {

    /**
    * Makes a Join SQL query for Oracle
    *
    * @param $table Table name
    * @param $alias Alias for the table
    * @param $condition Condition for the join
    * @return atkQuery The query object (for fluent usage).
    */
    function &addJoin($table, $alias, $condition, $outer)
    {
      if ($outer) $condition.=" (+)";
      $this->addtable($table,$alias);
      $this->addcondition($condition);
      return $this;
    }

    function _addLimiter(&$query)
    {
      /* limit? */
      if ($this->m_offset >= 0 && $this->m_limit > 0)
      {
        /* row id's start at 1! */
	      $query="SELECT * FROM (SELECT rownum AS rid, XX.* FROM (".
            $query.") XX) YY  WHERE YY.rid >= ".($this->m_offset+1)." AND YY.rid <= ".($this->m_offset + $this->m_limit);
      }
    }
    

   /**
    * Generate a searchcondition that checks whether $field contains $value .
    * 
    * This override adds special support for comparisons using a subquery instead
    * of a table field. Oracle doesn't allow direct UPPER(...) calls on the subquery
    * result so we need to wrap it inside a select query from dual. To prevent 
    * perform loss we try to detect if the comparison field is a subquery or not.  
    */
   function substringCondition($field, $value)
   {
     if (substr(ltrim(strtoupper($field), ' ('), 0, 7) != "SELECT ")
     {
       return parent::substringCondition($field, $value);
     }
     
     if($value[0]=='!')
     {
       return "(SELECT UPPER((".$field.")) FROM dual) NOT LIKE '%".strtoupper(substr($value,1,strlen($value)))."%'";
     }
     else
     {
       return "(SELECT UPPER((".$field.")) FROM dual) LIKE '%".strtoupper($value)."%'";
     }
   }     
 }
?>