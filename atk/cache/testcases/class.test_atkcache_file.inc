<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Test class for file based caching
 *
 * @package atk
 * @subpackage cache
 *
 * @copyright (c)2008 Sandy Pleyte
 * @author Sandy Pleyte <sandy@achievo.org>
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5898 $
 * $Id: class.test_atkcache_file.inc 5898 2008-04-27 19:51:11Z guido $
 */
atkimport("atk.test.atkcachetestcase");

class test_atkcache_file extends atkCacheTestCase
{
   protected $m_cache;

   protected $m_type= "file";
}
?>