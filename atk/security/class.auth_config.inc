<?php
/**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 4607 $
   * $Id: class.auth_config.inc 4607 2007-02-05 16:40:53Z ivo $
   */

  /**
   * Driver for authentication and authorization using entries in the
   * configurationfile.
   *
   * See the methods in the atkConfig class for an explanation of how to add
   * users and privileges.
   *
   * Does not support authorization.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   *
   */
   class auth_config extends auth_interface
   {
     function validateUser($user, $passwd)
     {
       if ($user=="") return AUTH_UNVERIFIED; // can't verify if we have no userid

       global $config_user;
       if ($user!="" && $passwd!="" && $config_user[$user]["password"]==$passwd)
       {
         return AUTH_SUCCESS;
       }
       else
       {
         return AUTH_MISMATCH;
       }
     }

     function canMd5()
     {
       return atkconfig("authentication_md5");
     }

     function getUser($user)
     {
       global $config_user;
       return Array("name"=>$user, "level"=>$config_user[$user]["level"]);
     }

     function getEntity($node, $action)
     {
       global $config_access;

       $rights = $config_access[$node];

       $result = Array();

       for ($i=0;$i<count($rights);$i++)
       {
         if ($rights[$i][$action]!="")
         {
           $result[] = $rights[$i][$action];
         }
         if ($rights[$i]["*"]!="")
         {
           $result[] = $rights[$i]["*"];
         }
       }

       return $result;
     }

     function getAttribEntity($node, $attrib, $mode)
     {
       global $config_attribrestrict;

       // $entity is an array of entities that may do $mode on $node.$attrib.
       $entity = $config_attribrestrict[$node][$attrib][$mode];

       return $entity;
     }


    function setPasswordAllowed()
     {
        return false;
     }
   }

?>
