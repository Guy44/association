<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 6053 $
 * $Id: class.atklanguage.inc 6053 2008-07-28 12:34:34Z dennis $
 */

/**
 * Class that handles userinterface internationalization.
 *
 * This class is used to retrieve the proper translations for any string
 * displayed in the userinterface. It includes only those language files
 * that are actually used, and has several fallback systems to find
 * translations if they can be find in the correct module.
 *
 * @author Boy Baukema <boy@ibuildings.nl>
 * @package atk
 *
 */
class atkLanguage
{
	/**
	 * Directory where language files are stored.
	 * @access private
	 * @var String
	 */
	var $LANGDIR = "languages/";

	/**
	 * Contains all currently loaded language strings.
	 * @access private
	 * @var array
	 */
	var $m_cachedlang = array();

	/**
	 * List of currently loaded language files
	 * @access private
	 * @var array
	 */
	var $m_cachedlangfiles = array();

	/**
	 * List of fallback modules
	 * @access private
	 * @var array
	 */
	var $m_fallbackmodules = array();

	/**
	 * List of override modules
	 * @access private
	 * @var array
	 */
	var $m_overridemodules = array("langoverrides");

	/**
	 * List of custum language string overrides
	 * @access private
	 * @var array
	 */
	var $m_customStrings = array();

	/**
	 * Default Constructor
	 * @access private
	 */
	function atkLanguage()
	{
		atkdebug("New instance made of atkLanguage");
	}

	/**
	 * Gets an instance of the atkLanguage class
	 *
	 * Using this function will ensure that only 1 instance ever exists
	 * (singleton).
	 *
	 * @return atkLanguage Instance of the atkLanguage class
	 */
	function &getInstance()
	{
		static $s_atklanguage;
		if (!is_object($s_atklanguage))
		{
			$s_atklanguage = new atkLanguage();
		}
		return $s_atklanguage;
	}

	/**
	 * Add a module that serves as an override for language strings.
	 *
	 * @param String $module Name of the module to add.
	 */
	function addOverrideModule($module)
	{
		array_unshift($this->m_overridemodules,$module);
	}

	/**
	 * Add a module that servers as a fallback for language strings.
	 *
	 * @param String $module Name of the module to add.
	 */
	function addFallbackModule($module)
	{
		$this->m_fallbackmodules[] = $module;
	}

	/**
	 * Calculate the list of fallbackmodules.
	 * @access private
	 */
	function _getFallbackModules($modulefallback)
	{
		static $s_fallbackmodules = array();
		$key = $modulefallback ? 1 : 0; // we can be called with true or false, cache both results

		if (!array_key_exists($key, $s_fallbackmodules))
		{
			global $g_modules;

			$modules = array();
			if (is_array($g_modules) && ($modulefallback|| atkconfig("language_modulefallback",false)))
			{
				foreach ($g_modules as $modname => $modpath)
				{
					$modules[] = $modname;
				}
			}
			$modules[] = "atk";

			$s_fallbackmodules[$key] = array_merge($this->m_fallbackmodules, $modules);
		}

		return $s_fallbackmodules[$key];
	}

	/**
	 * Text function, retrieves a translation for a certain string.
	 *
	 * @static
	 * @param mixed $string           string or array of strings containing the name(s) of the string to return
	 *                                when an array of strings is passed, the second will be the fallback if
	 *                                the first one isn't found, and so forth
	 * @param String $module          module in which the language file should be looked for,
	 *                                defaults to core module with fallback to ATK
	 * @param String $node            the node to which the string belongs
	 * @param String $lng             ISO 639-1 language code, defaults to config variable
	 * @param String $firstfallback   the first module to check as part of the fallback
	 * @param boolean $nodefaulttext  if true, then it doesn't returns false when it can't find a translation
	 * @param boolean $modulefallback Wether or not to use all the modules of the application in the fallback,
	 *                                when looking for strings
	 * @return String the string from the languagefile
	 */
	function text($string, $module, $node="", $lng="", $firstfallback="", $nodefaulttext=false,$modulefallback=false)
	{
		// We don't translate nothing
		if ($string=='') return '';
		if ($lng=="") $lng = atkLanguage::getLanguage();
		$lng = strtolower($lng);

		$atklanguage =& atkLanguage::getInstance();

		// If only one string given, process it immediatly
		if (!is_array($string))
		// GG
		//     return $atklanguage->_getString($string, $module, $lng, $node, $nodefaulttext, $firstfallback, $modulefallback);
		return $atklanguage->_getStringDb($string, $module, $lng, $node, $nodefaulttext, $firstfallback, $modulefallback);
		// GG FIN
		// If multiple strings given, iterate through all strings and return the translation if found
		for ($i = 0, $_i = count($string); $i < $_i; $i++)
		{
			// Try to get the translation
			// GG
			//      $translation = $atklanguage->_getString($string[$i], $module, $lng, $node, $nodefaulttext || ($i < ($_i-1)), $firstfallback, $modulefallback);
			$translation = $atklanguage->_getStringDb($string[$i], $module, $lng, $node, $nodefaulttext || ($i < ($_i-1)), $firstfallback, $modulefallback);
			// GG F
			// Return the translation if found
			if ($translation != "")
			return $translation;
		}
		return "";
	}

	/**
	 * Get the current language, either from url, or if that's not present, from what the user has set.
	 * @static
	 * @return String current language.
	 */
	function getLanguage()
	{
		global $ATK_VARS;

		if (isset($ATK_VARS["atklng"]) && in_array($ATK_VARS["atklng"],atkLanguage::getSupportedLanguages()))
		{
			$lng = $ATK_VARS["atklng"];
		} // we first check for an atklng variable
		else { $lng = atkLanguage::getUserLanguage(); }
		return strtolower($lng);
	}

	/**
	 * Change the current language.
	 * Note that his only remains set for the current request, it's not
	 * session based.
	 * @static
	 * @param String $lng The language to set
	 */
	function setLanguage($lng)
	{
		global $ATK_VARS;
		$ATK_VARS["atklng"] = $lng;
	}

	/**
	 * Get the selected language of the current user if he/she set one,
	 * otherwise we try to get it from the browser settings and if even THAT
	 * fails, we return the default language.
	 *
	 * @static
	 * @return unknown
	 */
	function getUserLanguage()
	{
		$supported = atkLanguage::getSupportedLanguages();

		$sessionmanager = &atkGetSessionManager();
		if (!empty($sessionmanager))
		{
			if (function_exists("getUser"))
			{
				$userinfo = getUser();
				// GG
				$fieldname = atkconfig('auth_languagefieldid');
				if (isset($userinfo[$fieldname]) && in_array($userinfo[$fieldname],$supported))
				{
					// get language code from table type_langue

					return $userinfo[$fieldname];
				}
				// GG FIN
				$fieldname = atkconfig('auth_languagefield');
				if (isset($userinfo[$fieldname]) && in_array($userinfo[$fieldname],$supported)) return $userinfo[$fieldname];

			}
		}

		// Otherwise we check the headers
		if (atkconfig('use_browser_language', false))
		{
			$headerlng = atkLanguage::getLanguageFromHeaders();
			if ($headerlng && in_array($headerlng,$supported)) return $headerlng;
		}

		// We give up and just return the default language
		return atkconfig('language');
	}

	/**
	 * Get the primary languagecode that the user has set in his/her browser
	 *
	 * @static
	 * @return String The languagecode
	 */
	function getLanguageFromHeaders()
	{
		if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE']))
		{
			$langs = split ('[,;]',$_SERVER['HTTP_ACCEPT_LANGUAGE']);
			if ($langs[0]!="")
			{
				$elems=explode("-", $langs[0]); // lng might contain a subset after the dash.
				$autolng=$elems[0];
			}
		}
		return $autolng;
	}

	/**
	 * Get the languages supported by the application
	 *
	 * @static
	 * @return Array An array with the languages supported by the application.
	 */
	function getSupportedLanguages()
	{
		static $s_supported = array();

		$supportedlanguagesmodule = atkconfig('supported_languages_module');
		if (empty($s_supported) && $supportedlanguagesmodule)
		{
			$supportedlanguagesdir = atkLanguage::getLanguageDirForModule($supportedlanguagesmodule);
			atkimport('atk.utils.atkdirectorytraverser');
			$supportedlanguagescollector = new getSupportedLanguagesCollector();
			$traverser = new atkDirectoryTraverser();
			$traverser->addCallbackObject($supportedlanguagescollector);
			$traverser->traverse($supportedlanguagesdir);
			$s_supported = $supportedlanguagescollector->getLanguages();
		}
		return $s_supported;
	}

	/**
	 * Determine the list of modules we need to go through to check
	 * language strings. Overrides have precedence, then the
	 * passed module is considered, finally if no string is found
	 * the fallbacks are checked.
	 *
	 * @access private
	 * @param String $module manually passed module
	 * @param String $firstfallback an additional module in which the
	 *        translation will be searched first, if not found in the
	 *        module itself.
	 * @param Boolean $modulefallback If true, *all* modules are checked.
	 * @return array List of modules to use to find the translations
	 */
	function _getModules($module, $firstfallback="", $modulefallback=false)
	{
		$arr = array();
		if ($module) $arr[] = $module;
		if ($firstfallback!="") $arr[] = $firstfallback;
		$modules = array_merge($this->m_overridemodules, $arr, $this->_getFallbackModules($modulefallback));
		return $modules;
	}

	/**
	 * This function takes care of the fallbacks when retrieving a string ids.
	 * It is as following:
	 * First we check for a string specific to both the module and the node
	 * (module_node_key).
	 * If that isn't found we check for a node specific string (node_key).
	 * And if all that fails we look for a general string in the module.
	 *
	 * @access private
	 * @param string $string        the name of the string to return
	 * @param string $module        module in which the language file should be looked for,
	 *                              defaults to core module with fallback to ATK
	 * @param string $node          the node to which the string belongs
	 * @param string $lng           ISO 639-1 language code, defaults to config variable
	 * @param bool  &$nodefaulttext wether or not to pass a default text back
	 * @return string the name with which to call the string we want from the languagefile
	 */
	function _getString($key, $module, $lng, $node="", $nodefaulttext=false, $firstfallback="", $modulefallback=false)
	{
		// first find node specific string.
		$modules = $this->_getModules($module, $firstfallback, $modulefallback);

		// First check custom Strings
		if(isset($this->m_customStrings[$lng]) && isset($this->m_customStrings[$lng][$key]))
		return $this->m_customStrings[$lng][$key];

		if ($node!="")
		{
			foreach ($modules as $modname)
			{
				$text = $this->_getStringFromFile($module."_".$node."_".$key, $modname, $lng);
				if ($text!="") return $text;
			}

			foreach ($modules as $modname)
			{
				$text = $this->_getStringFromFile($node."_".$key, $modname, $lng);
				if ($text!="") return $text;
			}
		}

		// find generic module string
		foreach ($modules as $modname)
		{
			$text = $this->_getStringFromFile($key, $modname, $lng);
			if ($text!="") return $text;
		}

		if (!$nodefaulttext)
		{
			if (atkconfig("debug_translations", false))
			atkdebug("atkLanguage: translation for '$key' with module: '$module' and node: '$node' and language: '$lng' not found, returning default text");

			// Still nothing found. return default string
			return $this->defaultText($key);
		}
		return "";
	}
	// GG
	/**
	 * This function takes care of the fallbacks when retrieving a string ids.
	 * It is as following:
	 * First we check for a string specific to both the module and the node
	 * (module_node_key).
	 * If that isn't found we check for a node specific string (node_key).
	 * And if all that fails we look for a general string in the module.
	 *
	 * @access private
	 * @param string $string        the name of the string to return
	 * @param string $module        module in which the language file should be looked for,
	 *                              defaults to core module with fallback to ATK
	 * @param string $node          the node to which the string belongs
	 * @param string $lng           ISO 639-1 language code, defaults to config variable
	 * @param bool  &$nodefaulttext wether or not to pass a default text back
	 * @return string the name with which to call the string we want from the languagefile
	 */
	function _getStringDb($key, $module, $lng, $node="", $nodefaulttext=false, $firstfallback="", $modulefallback=false)
	{

		// first find node specific string.
		$modules = $this->_getModules($module, $firstfallback, $modulefallback);
		/*		if (substr_count  ( $key , "_tooltip" )>0)

		{ echo $key;
		echo '</br>';
		echo $module;
		echo '</br>';
		echo $node;
		echo '</br>';
		echo $lng;
		echo '</br>';
		die();
		}
		*/

		if ($node=="") $module="";

		// First check custom Strings
		if(isset($this->m_customStrings[$lng]) && isset($this->m_customStrings[$lng][$key]))
		return $this->m_customStrings[$lng][$key];

		//        $text = $this->_getStringFromFile($module."_".$node."_".$key, $modname, $lng);
		// $type_traduction : menu, attribut, atk , wiki ?
		// $type_texte : texte_traduit ou texte_tooltip ?
		//
		// menu_tooltip_ -> $type_traduction=menu (5) $tooltip=true
		// menu_ -> $type_traduction=menu (5) $tooltip=false

		// ...._tooltip -> $type_traduction=attribut (2)$tooltip=true
		// atkattribute  $ttip = atktext($node->m_type."#".$this->fieldName()."_tooltip", $module, "", "", "", true);
		//  if (!$module) $module = "atk";
		//GG utilise # � la place de _
		//   $ttip = atktext($node->m_type."#".$field["name"]."_tooltip", $module, "", "", "", true);
		 
		// atktabbedpane : $ttip = atktext($node->m_type."#".$field["name"]."_tooltip", $module, "", "", "", true);
		//if (!$module) $module = "atk";
		//   $ttip = atktext($node->m_type."_".$name."_tooltip", $module, "", "", "", true);
			

		// wiki_ -> $type_traduction=wiki (6)$tooltip=false
		// rien de sp�cial -> $type_traduction=atk (1) ou application (4) $tooltip=false

		/*	if (substr_count  ( $key , "_tooltip" )>0)
		 {
		 echo $g_traduction_tt[$module.$separateur.$id_lng.$separateur.$node.$separateur.$type_traduction.$separateur.$key]."<br/>";
		 echo $g_traduction_tt["".$separateur.$id_lng.$separateur."".$separateur."5".$separateur.$key]."<br/>";
		 die ();}
		 */

			
		if (  substr_count  ( $key , "menu_tooltip_") >0 )
		{
			$key=str_replace("menu_tooltip_","",$key);
			$type_traduction="5";
			$tooltip=true;
		}
		elseif (  substr_count  ( $key , "menu_tooltip_" )==0 && substr_count  ( $key , "menu_" )>0)
		{
			$key=str_replace("menu_","",$key);
			$type_traduction="5";
			$tooltip=false;
		}
		elseif (  substr_count  ( $key , "_tooltip" )>0  && substr_count  ( $key , "menu_tooltip" )==0)
		{
			$key=str_replace("_tooltip","",$key);
			$node=substr($key,0,strpos($key,"#"));
			$key=substr($key,strpos($key,"#")+1);
			$type_traduction="2";
			$tooltip=true;
		}
		elseif (  substr_count  ( $key , "wiki_") >0 )
		{
			$key=str_replace("wiki_","",$key);
			$type_traduction="6";
			$tooltip=false;
		}
		/*		elseif (  substr_count  ( $key , "action_") >0 )
		 {
			$key=str_replace("action_","",$key);
			$type_traduction="3";
			$tooltip=false;
			}
			*/
		elseif ( $node!="" && $module!="" )
		{

			$type_traduction="2";
			$tooltip=false;
		}
		else
		{
			$type_traduction="1";
			$tooltip=false;
		}
		if (atkGetUser()!="")
		{
			global $g_trace_atklanguage;
			//$g_trace_atklanguage= "";
			global $g_sessionManager;
			/*		$cle1="";
			 $cle2="";
			 $cle2bis="";
			 $cle3="";
			 */
			$separateur=" @|* ";
				
				
			$g_traduction_texte = $g_sessionManager->getValue("traduction_texte", "globals");
			$g_traduction_tt = $g_sessionManager->getValue("traduction_tt", "globals");
			//if (!$nodefaulttext) return "";

			$id_lng= $g_sessionManager->getValue("id_langue", "globals");

			if ($tooltip==true && $node!=""  && $g_traduction_tt[$module.$separateur.$id_lng.$separateur.$node.$separateur.$type_traduction.$separateur.$key]!="")
			{
				return $g_traduction_tt[$module.$separateur.$id_lng.$separateur.$node.$separateur.$type_traduction.$separateur.$key];
			}
			if ($tooltip==true && $node==""  && $g_traduction_tt["".$separateur.$id_lng.$separateur."".$separateur."5".$separateur.$key]!="")
			{
				return $g_traduction_tt["".$separateur.$id_lng.$separateur."".$separateur."5".$separateur.$key];
			}
			if ($tooltip==true)
			{ return NULL; }


			if ( $tooltip==false && ($key=="..." || $key=="#")) { return $key;}
			// Pr�fixes utilis�s dans atk/ui/atkui.inc
			if ($tooltip==false && (substr($key, 0,6)=="title_" || substr($key, 0,4)=="tab_"|| substr($key, 0,7)=="action_" || substr($key, 0,5)=="link_"  || substr($key, 0,8)=="topmenu_"))
			{
				if (substr($key, 0,6)=="title_")
				{
					$key=substr($key, 6, strlen($key)-6);
					return $g_traduction_texte["".$separateur.$id_lng.$separateur."".$separateur."5".$separateur.$key];
				}
				if (substr($key, 0,7)=="action_")
				{
					$key=substr($key, 7, strlen($key)-7);
				//	return $g_traduction_tt["".$separateur.$id_lng.$separateur."".$separateur."3".$separateur.$key];
					return $g_traduction_texte["".$separateur.$id_lng.$separateur."".$separateur."3".$separateur.$key];
				}
				if (substr($key, 0,5)=="link_")
				{
					$key=substr($key, 5, strlen($key)-5);
					return $g_traduction_texte["".$separateur.$id_lng.$separateur."".$separateur."5".$separateur.$key];
				}
				return "";
				//		return $this->defaultText($key);
			}
				
			$cle1=$module.$separateur.$id_lng.$separateur.$node.$separateur.$type_traduction.$separateur.$key;
			if ($tooltip==false && $g_traduction_texte[$cle1]!="")
			{
				return $g_traduction_texte[$cle1];
			}
			$cle2="".$separateur.$id_lng.$separateur."".$separateur."4".$separateur.$key;
			if ($tooltip==false && $g_traduction_texte[$cle2]!="")
			{
				return $g_traduction_texte[$cle2];
			}
			$cle2bis="".$separateur.$id_lng.$separateur."".$separateur."1".$separateur.$key;
			if ($tooltip==false && $g_traduction_texte[$cle2bis]!="")
			{
				return $g_traduction_texte[$cle2bis];
			}
			$cle3="".$separateur.$id_lng.$separateur."".$separateur."5".$separateur.$key;
			if ($tooltip==false && $g_traduction_texte[$cle3]!="")
			{
				return $g_traduction_texte[$cle3];
			}
			if ($tooltip==false &&  substr($key, 0,12)=="descripteur_")
			{return $this->defaultText($key);
			}
			return $key;
		}

		$text = $this->_getStringFromDb($key, $module, $lng, $node, $type_traduction, $tooltip);
		if ($text!="")
		{return $text;}
		else
		{
			$text = $this->_getStringFromDb($key, "", $lng, "", "1", false);
			if ($text!="")
			{
				return $text;
			}
		}
		if (!$nodefaulttext)
		{
			if (atkconfig("debug_translations", false))
			atkdebug("atkLanguage: translation for '$key' with module: '$module' and node: '$node' and language: '$lng' not found, returning default text");

			// Still nothing found. return default string
			return $this->defaultText($key);
		}
		return "";
	}

	// GG FIN
	/**
	* Checks wether the language is set or not
	*
	* If set, it does nothing and return true
	* otherwise it sets it
	*
	* @access private
	* @param string $file    file to import
	* @param string $module  the module to import the language file from
	* @param string $lng     language of file to import
	* @return bool true if everything went okay
	*/
	function _includeLanguage($module, $lng)
	{
		if (!isset($this->m_cachedlangfiles[$module][$lng])||$this->m_cachedlangfiles[$module][$lng] != 1)
		{
			$this->m_cachedlangfiles[$module][$lng] = 1;
			$path = $this->getLanguageDirForModule($module);

			$file = $path.$lng.".lng";

			if (file_exists($file))
			{
				include($file);
				$this->m_cachedlang[$module][$lng] = $$lng;
				return true;
			}
			return false;
		}
		return true;
	}

	/**
	 * Method for getting the relative path to the languagedirectory
	 * of a module.
	 * Supports 2 special modules:
	 * - atk (returns the path of the atk languagedir)
	 * - langoverrides (returns the path of the languageoverrides dir)
	 *
	 * Special method in that it can run both in static and non-static
	 * mode.
	 *
	 * @param String $module The module to get the languagedir for
	 * @return String The relative path to the languagedir
	 */
	function getLanguageDirForModule($module)
	{
		if ($module=="atk")
		{
			$path = atkconfig("atkroot")."atk/".(isset($this)?$this->LANGDIR:'languages/');
		}
		else if ($module=="langoverrides")
		{
			$path = atkconfig("language_basedir",(isset($this)?$this->LANGDIR:'languages/'));
		}
		else
		{
			$path = moduleDir($module).(isset($this)?$this->LANGDIR:'languages/');
		}
		return $path;
	}

	/**
	 * A function to change the original "$something_text" string to
	 * "Something text"
	 * This is only used when we really can't find the "$something_text" anywhere
	 * @param string $string the name of the string to return
	 * @return string the changed string
	 */
	function defaultText($string)
	{
		return ucfirst(str_replace("_"," ",str_replace('title_','',$string)));
	}

	/**
	 * Gets the string from the languagefile or, if we failed, returns ""
	 *
	 * @access private
	 * @param string $string        the name which was given when the text function was called
	 * @param string $module        the name of the module to which the text function belongs
	 * @param string $node          the name of the node to which the text function belongs
	 * @param string $lng           the current language
	 * @param bool  &$nodefaulttext wether or not to pass a default text back
	 * @return var the true name by which the txt is called or "" if we can't find any entry
	 */
	function _getStringFromFile($key, $module, $lng)
	{
		$this->_includeLanguage($module, $lng);

		if (isset($this->m_cachedlang[$module])
		&& is_array($this->m_cachedlang[$module][$lng])
		&& isset($this->m_cachedlang[$module][$lng][$key]))
		{
			return $this->m_cachedlang[$module][$lng][$key];
		}
		return "";
	}

	// GG

	/**
	 * Gets the string from the languagefile or, if we failed, returns ""
	 *
	 * @access private
	 * @param string $string        the name which was given when the text function was called
	 * @param string $module        the name of the module to which the text function belongs
	 * @param string $node          the name of the node to which the text function belongs
	 * @param string $lng           the current language
	 * @param bool  &$nodefaulttext wether or not to pass a default text back
	 * @return var the true name by which the txt is called or "" if we can't find any entry
	 *
	 */
	function _getStringFromDb ($key, $modname, $lng, $nodename, $type_traduction, $tooltip)
	{
		//return "";
		//		atkdebug("atkLanguage: translation for '$key' with module: '$modname' and node: '$nodename' and language: '$lng' type trad : '$type_traduction' et type tooltip :'$tooltip' ");

		$key = str_replace('"','\"',$key);
		$sql= 'SELECT ';
		if ($tooltip==true)
		{
			$sql.=' texte_tt as texte ';
		}
		else
		{
			$sql.=' texte_traduit as texte ';
		}
		$sql.=' FROM app_traduction
		WHERE id_type_langue in (select id from type_langue where
		code_iso_639_1="'.$lng.'" ) ';
		if ($type_traduction!='6')
		{$sql.=' AND texte_source ="'. $key .'" ';}

		if 	($nodename!="")
		{
			$sql.=' AND nom_noeud="'.$nodename .'" ';
		}

		if 	($modname!="")
		{
			$sql.=' AND nom_module="'.$modname .'" ';
		}
		if 	($type_traduction!="1")
		{
			$sql.=' AND id_type_traduction="'.$type_traduction .'" ';
		}
		else
		{
			$sql.=' AND id_type_traduction IN ("1","4","6") ';
		}
		$sql.='   ';
		/*		if ($key=='id_titre')
		 {
		 echo '<br> $key '.$key.' mod : '.$modname.' lng : '.$lng.' node : '.$nodename.' typetrad :  '.$type_traduction.' tip :  '.$tooltip;
		 echo '<br></br>';
		 echo $sql.'<br></br>';
		 die();
		 }
		 */
		$db = & atkGetDb();
		$data = $db->getrows($sql);
		if (count($data) == 0)
		{return "";}
		else
		{return $data[0]["texte"];}

	}

	// GG FIN

	/**
	 * Set a custom language string
	 *
	 * @param string $code The code of the custom string
	 * @param string $text Text
	 * @param string $lng Language
	 */
	function setText($code,$text,$lng)
	{
		if(!isset($this->m_customStrings[$lng])) $this->m_customStrings[$lng]=array();
		$this->m_customStrings[$lng][$code]=$text;
	}
}


/**
 * A collector for supported languages
 * @author Boy Baukema <boy@ibuildings.nl>
 * @package atk
 */
class getSupportedLanguagesCollector
{
	var $m_languages=array();

	function visitFile($fullpath)
	{
		if (substr($fullpath,strlen($fullpath)-4)==='.lng')
		{
			$exploded = explode('/',$fullpath);
			$lng = array_pop($exploded);
			$this->m_languages[] = substr($lng,0,2);
		}
	}

	function getLanguages()
	{
		return $this->m_languages;
	}
}
?>
