<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Test class for Zend Platform (disk)
 *
 * @package atk
 * @subpackage cache
 *
 * @copyright (c)2008 Sandy Pleyte
 * @author Sandy Pleyte <sandy@achievo.org>
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5898 $
 * $Id: class.test_atkcache_zp_disk.inc 5898 2008-04-27 19:51:11Z guido $
 */
atkimport("atk.test.atkcachetestcase");

class test_atkcache_zp_disk extends atkCacheTestCase
{
   protected $m_cache;

   protected $m_type= "zp_disk";

   public function skip()
   {
     $this->skipIf(!function_exists('accelerator_license_info'),'The Zend Platform extension is not loaded');
     if (function_exists('accelerator_license_info') && !function_exists('accelerator_get_configuration'))
     {
       $licenseInfo = accelerator_license_info();
       $this->skipIf(!function_exists('accelerator_get_configuration'),'The Zend Platform extension is not loaded correctly: '.$licenseInfo['failure_reason']);
     }
     $this->skipIf(!function_exists('zend_disk_cache_store'),'The Zend Platform doesn\'t support the disk cache (version 3.6 and higher only)');
   }
}
?>