"use strict";

// variables

var Fake = {};
var Overlay = {}; // Overlay
var Underlay = {}; // Underlay
var Device = {
	type: "unknown",
	name: "unknown",
	isMobile: false,
	isDesktop: false,
	vertical: false,
	horizontal: false,
	ratio: null
};
var Gesture = {
	// touch start / end
	start_x : 0,
	start_y : 0,
	end_x : 0,
	end_y : 0,
	Rec : ""
};

var Scroll = {
	keys : [32,33,34,35,36,37,38,39,40]
};

// DESKTOP
// spacebar: 32, pageup: 33, pagedown: 34, end: 35, home: 36
// left: 37, up: 38, right: 39, down: 40,
// (Source: http://stackoverflow.com/a/4770179)

/********************
 * DOM MANIPULATION *
 ********************/

var Placeholder = {
	s1: "https://i.imgur.com/jiKi1KW.png",
	s2: "https://i.imgur.com/2tAvXxg.png",
// 	m1: "https://lh5.googleusercontent.com/JkdPGhPBWQJYdd7w7QYYjp5IMZx0gaikk2L5V2CDNOa2FrbqfEn_WBfyZ_PAZ7dkKIs8I3_sAUVaD9GrpO7o=w1920-h882",
// 	m2: "https://lh3.googleusercontent.com/en8wnLhL9-30zMhg74Z4RsuZvvktoa6ugnaHITVPoldsxw0U-jgcI-JnMxqNuobCx40kIQU6_xPJVaBFt3BA=w1920-h882"
}

// svg transform fix for IE / Edge
void(new MutationObserver(function(muts) {
	for(let i = muts.length; i--;) {
		let mut = muts[i], objs = mut.target.querySelectorAll('foreignObject');
		for(let j = objs.length; j--;) {
				let obj = objs[j];
				let val = obj.style.display;
				obj.style.display = 'none';
				obj.getBBox();
				obj.style.display = val;
		}
	}
}).observe(document.documentElement, { attributes: true, attributeFilter: ['transform'], subtree: true }));

// font sizing, same as sass function for real time calculation
function sizing(o) { // if yes, will return 100% - result
	SetDefaults(o, { $font_sizing: true, $sign: "*", $factor: 1, $from_max: false });
	let $sign = isString(o.$sign) ? o.$sign : "*";
	let $from_max = isSet(o.$from_max) ? o.$from_max : false;
	let $factor = isNumber(o.$factor) ? o.$factor : 1;
	let $base_rem = "0rem !default";
	let $base_vw = "0vw !default";
	let $base_vh = "0vh !default";
	if (o.$font_sizing) { // better for mobile
		$base_rem: "0.5rem";
		$base_vw: "0.3vw";
		$base_vh: "0.6vh";
	} else {
		$base_rem: "0.62rem";
		$base_vw: "1vw";
		$base_vh: "1.2vh";
	}
	if (o.$sign == "/") {
		if (o.$from_max) {
			return `calc(100% - ((${$base_rem} + ${$base_vw} + ${$base_vh}) / ${$factor}))`;
		}
		return `calc((${$base_rem} + ${$base_vw} + ${$base_vh}) / ${$factor})`;
	} else if ($sign == "*") {
		if ($from_max) {
			return `calc(100% - ((${$base_rem} + ${$base_vw} + ${$base_vh}) * ${$factor}))`;
		}
		return `calc((${$base_rem} + ${$base_vw} + ${$base_vh}) * ${$factor})`;
	}
}
// CSS insertion
var RuntimeStyle = document.createElement('style');

document.head.appendChild(RuntimeStyle);
var CSS_AddKeyFrames = null;
if (CSS && CSS.supports && CSS.supports('animation: name')){
	// we can safely assume that the browser supports unprefixed version.
	CSS_AddKeyFrames = function(name, s){
			CSS_Insert("@keyframes " + name, s);
	}
} else {
	CSS_AddKeyFrames = function(name, s){
		// Ugly and terrible, but users with this terrible of a browser
		// *cough* IE *cough* don't deserve a fast site
		let str = name + s;
		let pos = RuntimeStyle.length;
		RuntimeStyle.sheet.insertRule("@-webkit-keyframes " + str, pos);
		RuntimeStyle.sheet.insertRule("@keyframes " + str, pos + 1); //not sure about that, need to test
	}
}

function CSS_Insert(name, s){ //use to insert class/id/keyframes to RuntimeStyle
	let pos = RuntimeStyle.length;
	RuntimeStyle.sheet.insertRule(name + s, pos);
}

// returns elements from a string
function htmlToElement(html) {
	var template = document.createElement('template');
	html = html.trim(); // Never return a text node of whitespace as the result
	if (!isSet(html)) { return null; }
	template.innerHTML = html;
	return template.content.firstChild;
}

// returns nodelist from a string
function htmlToNode(html) {
	var template = document.createElement('template');
	html = html.trim(); // Never return a text node of whitespace as the result
	if (!isSet(html)) { return null; }
	template.innerHTML = html;
	return template.content.childNodes;
}

function InsertBeforeFooter(html) {
	document.querySelector('footer').insertAdjacentHTML('beforebegin', html);
}

// add the RevealClass to the css:
// TODO: finish to implement reveal
// let reveal_right = `{
// 	transition: all .25s ease-in-out;
// 	opacity: 0;
// }`;
// CSS_Insert(".reveal", reveal_right);

// transform non agnostic 100vh to real viewport height on mobile:
function Fix100vhMobile(els) {
	if (isSet(Device) && isSet(Device.isMobile) && Device.isMobile) {
		// target only elements affected by the vh:
		els = GetElements(els);
		if (!els.length) { return; }
		for (let el of els) { el.style.height = Polyfill.windowHeight(); }
	}
}
// enable the elements with data-keyup-validate='[type,target]'
// to add data-err="error" to the target (selector) i.e. '["email","._form_field"]'
// TODO: modify to match jQuery version and use
function EnableDataKeyupValidate() {
	let els = GetElements("input[data-keyup-validate]");
	for (let el of els) {
		let arr = JSON.parse(el.getAttribute("data-keyup-validate").replace(/'/g, '"'));
		// type / target // data-keyup-validate='["email","._form_field"]'
		if (arr.length < 2) { return; } // wrong attr size
		let target = $(arr[1]);
		if (!target.length) { return; } // no match
		let fn_ptr = null;
		switch (arr[0]) {
			case "email":		fn_ptr = CheckEmail;	break;
			case "password":	fn_ptr = CheckPassword; break;
			case "name":		fn_ptr = CheckName;		break;
			case "mixed":		fn_ptr = CheckMixed;	break;
			// case "latin":		 fn_ptr = CheckLatin; break;
		}
		if (!isSet(fn_ptr)) { return; } // default or no function
		el.onkeyup(function(e) {
			let val = el.val();
			let check = true, err = null;
			[check, err] = fn_ptr(val);
			if (!check) { // invalid entry
				target.setAttribute("data-error", err);
				target.addClass("invalid");
				el.addClass("invalid");
				target.removeClass("valid");
				el.removeClass("valid");
			} else { // valid entry
				target.removeAttribute("data-error");
				target.removeClass("invalid");
				el.removeClass("invalid");
				target.addClass("valid");
				el.addClass("valid");
			}
		});
	}
}
// HTML insertion
// .appendBefore(element) Prototype
Element.prototype.appendBefore = function (el) {
	if (!isElement(el)) { el = GetElement(el); }
	if (!el) { return; }
	el.parentNode.insertBefore(this, el);
},false;

// .appendAfter(element) Prototype
Element.prototype.appendAfter = function (el) {
	if (!isElement(el)) { el = GetElement(el); }
	if (!el) { return; }
	el.parentNode.insertBefore(this, el.nextSibling);
},false;

Element.prototype.appendAsFirstChild = function(el) {
	if (!isElement(el)) { el = GetElement(el); }
	if (!el) { return; }
	el.insertBefore(this, el.childNodes[0]);
}

Element.prototype.isVisible = function(el) {
	let rect = GetElementRect(this);
	let height = Polyfill.windowHeight();
	// top el edge is visible OR bottom el edge is visible
	let topVisible = rect.top > 0 && rect.top < height;
	let bottomVisible = rect.bottom < height && rect.bottom > 0;

	return topVisible || bottomVisible;
	// return (el.offsetWidth > 0 && el.offsetHeight > 0); // doesn't work if element
}

Element.prototype.fullyVisible = function(el) {
	let rect = GetElementRect(this);
	let height = Polyfill.windowHeight();

	// top el edge is visible OR bottom el edge is visible
	let topVisible = rect.top > 0 && rect.top < height;
	let bottomVisible = rect.bottom < height && rect.bottom > 0;

	return topVisible && bottomVisible;
	// return (el.offsetWidth > 0 && el.offsetHeight > 0); // doesn't work if element
}

Element.prototype.halfVisible = function(el) {
	let rect = GetElementRect(this);
	let height = Polyfill.windowHeight();

	// top el edge is visible OR bottom el edge is visible
	let topVisible = rect.top > 0 && rect.top < height;
	let bottomVisible = rect.bottom < height && rect.bottom > 0;

	return topVisible && bottomVisible;
	// return (el.offsetWidth > 0 && el.offsetHeight > 0); // doesn't work if element
}

// jquery style event listeners
// callbacks take optional event as parameter: cb(event) when passed
Element.prototype.on = function(event, cb, o) { // useCapture = prioritize
	if (isBool(o)) {
		o = { useCapture: o, capture: o };
	} else if (!isObject(o)) { o = {}; } // construct object
	SetDefaults(o, {passive: true, useCapture: false, capture: false});
	this.addEventListener(event, cb, o);
}
Element.prototype.onclick = function(cb) { if (isFunction(cb)) { this.on("click", cb); } }
// triggers click when nothing passed, the jQuery way
Element.prototype.click = function(cb) { if (isFunction(cb)) { this.onclick(cb);	} else { this.click(); } }// DispatchClick(this);
Element.prototype.onkeydown = function(cb) { if (isFunction(cb)) { this.on("keydown", cb); } }
Element.prototype.keydown = function(cb) { if (isFunction(cb)) { this.onkeydown(cb);	} else { this.keydown(); } }// DispatchClick(this);
Element.prototype.onkeyup = function(cb) { if (isFunction(cb)) { this.on("keyup", cb); } }
// changes every time the element is infocused and the input is set
Element.prototype.onchange = function(cb) {	if (isFunction(cb)) { this.on("change", cb); } }
// changes every time a key is pressed
Element.prototype.oninput = function(cb) { if (isFunction(cb)) { this.on("input", cb); } }
Element.prototype.onblur = function(cb) { if (isFunction(cb)) { this.on("blur", cb); } }
Element.prototype.onfocus = function(cb) { if (isFunction(cb)) { this.on("focus", cb); } }
Element.prototype.onwheel = function(cb) { if (isFunction(cb)) { this.on("wheel", cb); } } // DOMMouseScroll / mousewheel
Element.prototype.onscroll = function(cb) { if (isFunction(cb)) { this.on("scroll", cb); } }
Element.prototype.ontouchmove = function(cb) { if (isFunction(cb)) { this.on("touchmove", cb); } }
Element.prototype.ontouchstart = function(cb) { if (isFunction(cb)) { this.on("touchstart", cb); } }
Element.prototype.ontouchend = function(cb) { if (isFunction(cb)) { this.on("touchstart", cb); } }
Element.prototype.onpopstate = function(cb) { if (isFunction(cb)) { this.on("popstate", cb); } }
// Element.prototype.onload = function(cb) { if (isFunction(cb)) { this.on("load", cb); } }
// Element.prototype.onDOMContentLoaded = function(cb) { if (isFunction(cb)) { this.on("DOMContentLoaded", cb); } }
// window related listeners
Window.prototype.on = function(event, cb, o = {passive: true, useCapture: false}) { // useCapture = prioritize
	if (isBool(o)) {
		o = { useCapture: o };
	}
	SetDefaults(o, {passive: true, useCapture: false});
	this.addEventListener(event, cb, o);
}
// document related listeners
HTMLDocument.prototype.on = function(event, cb, o = {passive: true, useCapture: false}) { // useCapture = prioritize
	if (isBool(o)) {
		o = { useCapture: o };
	}
	SetDefaults(o, {passive: true, useCapture: false});
	this.addEventListener(event, cb, o);
}

// jQuery document.ready implementation (faster)
function DocReady(f) { // function f is passed as callback
	if (!isFunction(f)) { return; }
	if (document.readyState === "complete" || (document.readyState !== "loading" && !document.documentElement.doScroll)) {
		f();
	} else {
		document.on("DOMContentLoaded", f);
	}
};

// class operation jQuery style
Element.prototype.addClass = function(c) {
	this.classList.add(c);
}
Element.prototype.removeClass = function(c) {
	this.classList.remove(c);
}
Element.prototype.hasClass = function(c) {
	return (this.classList.contains(c));
}
Element.prototype.toggleClass = function(c) {
	if (this.hasClass(c)) {
		this.removeClass(c);
	} else {
		this.addClass(c);
	}
}
// innerHTML jQuery style
Element.prototype.html = function(h) {
	if (isSet(h)) {
		this.innerHTML = h;
	} else {
		return this.innerHTML;
	}
}
Element.prototype.text = function(t) {
	if (isSet(t)) {
		this.innerText = t;
	} else {
		return this.innerText;
	}
}

// TODO: fix this
String.prototype.capitalize = function() {
	return this.replace(/(?:^|\s)\S/g, function(a) { return a.toUpperCase(); });
};

function strstr(haystack, needle, bool) {
    var pos = 0;

    haystack += "";
    pos = haystack.indexOf(needle); if (pos == -1) {
        return false;
    } else {
        if (bool) {
            return haystack.substr(0, pos);
        } else {
            return haystack.slice(pos);
        }
    }
}

// set defaults props to object (merge missing def to a)
function SetDefaults(o, def) {
	if (!(isObject(o) && isObject(def))) { return false; }
	for (let attr in def) {
		if (!isSet(o[attr]) && def.hasOwnProperty(attr)) {
			o[attr] = def[attr];
		}
	}
	return true;
}

// simulates a user click
Fake.Click = function(opt) {
	SetDefaults(opt, { el: null, el_x: null, el_y: null,
				screenX: 0, screenY: 0,
				clientX: 0, clientY: 0,
				ctrlKey: false, shiftKey: false,
				altKey: false, metaKey: false,
				button: 0 });
	// allow for use of x / y
	if (isInt(opt.x) && isInt(opt.y)) { opt.clientX = opt.x; opt.clientY = opt.y; }
	if (!(opt.key && isString(opt.key)) && !(opt.code && isInt(opt.code))) { return; } // no valid key
	if (isString(opt.key)) { opt.code = null; } // only use one
	let el = opt.el, el_x = opt.el_x, el_y = opt.el_y;
	if (el) { el = GetElement(el); }
	else if (el_x && el_x > 0 && el_y && el_y > 0) { el = GetFromCoordinates(el_x, el_y); }
	else { el = document; } // fire the event at the document scale
	let event = new MouseEvent("click", opt)
	el.dispatchEvent(event);
}

// simulates a user key stroke
Fake.KeyPress = function(opt) {
	SetDefaults(opt, { el: null, el_x: null, el_y: null,
				key: null, code: null, repeat: false,
				shiftKey: false, ctrlKey: false,
				altKey: false, metaKey: false,
				isComposing: false });
	// allow for use of x / y
	if (!(opt.key && isString(opt.key)) && !(opt.code && isInt(opt.code))) { return; } // no valid key
	if (isString(opt.key)) { opt.code = null; } // only use one
	let el = opt.el, el_x = opt.el_x, el_y = opt.el_y;
	if (el) { el = GetElement(el); }
	else if (el_x && el_x > 0 && el_y && el_y > 0) { el = GetFromCoordinates(el_x, el_y); }
	else { el = document; } // fire the event at the document scale
	let event = new KeyboardEvent("keypress", opt)
	el.dispatchEvent(event);
}

// simulates a user touch
// TODO: implement (slide and not just clicks)
Fake.Touch = function(opt = { el: null, x: null, y: null }) {
}

// fires either a touch or a click depending on current user device
Fake.Auto = function(opt) {
	// if (isSet(Device) && isSet(Device.isMobile) && Device.isMobile) {
	// 	return Fake.Touch(opt);
	// }
	return Fake.Click(opt);
}

/***********
 * GESTURE *
 ***********/

// requestAnimationFrame for Smart Animating http://goo.gl/sx5sts
// there are plenty of polyfills for non compatible browsers

function GetFromCoordinates(x, y, z = null) { return document.elementFromPoint(x,y); }
function GetActive() { return document.activeElement; }
function GetElements(el) { // can select from string if no element is passed
	if (!isNodeList(el) && !isHTMLCollection(el)) { // nodeType 1 == actual element
		if (isNode(el)) { return [el]; }
		el = document.querySelectorAll(el);
		if (!isNodeList(el) && !isHTMLCollection(el) && !isArray(el)) { return null; }
	}
	return el;
}

// .appendAfter(element) Prototype
Element.prototype.find = function (el) {
	el = this.querySelector(el);
	if (!isNode(el) && !isElement(el)) { return null; }
	return el;
},false;

function GetElement(el) {
	if (isNode(el)) { return el; }
	let all = GetElements(el);
	return isSet(all) ? all[0] : null;
}

function GetElementRect(el) {
	el = GetElement(el); if (!el) { return; }
	return el.getBoundingClientRect();
}

var Polyfill = {};

Polyfill.windowWidth = function() {
	let poly =
		(((window.innerWidth && document.documentElement.clientWidth)
		? Math.min(window.innerWidth, document.documentElement.clientWidth)
		: window.innerWidth)
		|| document.documentElement.clientWidth
		|| 0);
		return poly;
}

Polyfill.windowHeight = function() {
	let poly =
		(((window.innerHeight && document.documentElement.clientHeight)
		? Math.min(window.innerHeight, document.documentElement.clientHeight)
		: window.innerHeight)
		|| document.documentElement.clientHeight
		|| 0);
		return poly;
}

Polyfill.docWidth = function() {
	let poly =
		(document.body.offsetWidth
		|| document.getElementsByTagName('body')[0].clientWidth
		|| 0);
		return poly;
}

Polyfill.docHeight = function() {
	let poly =
		(document.body.offsetHeight
		|| document.getElementsByTagName('body')[0].clientHeight
		|| 0);
		return poly;
}

Polyfill.scrollTop = function(px) {
	if (typeof px === "number") { // setter
		document.body.parentNode.scrollTop =
		// window.pageYOffset =
		document.documentElement.scrollTop =
		document.body.scrollTop = px;
		return;
	}
	let poly = // getter
		(document.body.parentNode.scrollTop
		// || window.pageYOffset
		|| document.documentElement.scrollTop
		|| document.body.scrollTop
		|| 0);
		return poly;
}

Polyfill.scrollLeft = function(px) {
	if (typeof px === "number") { // setter
		document.body.parentNode.scrollLeft =
		// window.pageXOffset =
		document.documentElement.scrollLeft =
		document.body.scrollLeft = px;
		return;
	}
	let poly = // getter
		(document.body.parentNode.scrollLeft
		// || window.pageXOffset
		|| document.documentElement.scrollLeft
		|| document.body.scrollLeft
		|| 0);
		return poly;
}

// auto pollyfill for requestAnimFrame
var requestAnimFrame = (function() {
	return window.requestAnimationFrame
		|| window.webkitRequestAnimationFrame
		|| window.mozRequestAnimationFrame
		|| function(cb) { window.setTimeout(cb, 1000 / 60); };
})();

// (type, bubbles, cancelable, viewArg, 
//        ctrlKeyArg, altKeyArg, shiftKeyArg, metaKeyArg, 
//            keyCodeArg, charCodeArg)

// auto polyfill for MouseEvent and KeyboardEvent
(function (window) {
	if (typeof KeyboardEvent == "function") { return; } // no need
    // Polyfills DOM4 KeyboardEvent
	var KeyboardEventPolyfill = function (type, o) {
		o = o || { bubbles: true, cancelable: true };
		var keyboardEvent = document.createEvent('KeyboardEvent');
		keyboardEvent.initMouseEvent(type, 
			o.bubbles,
			o.cancelable,
			window,
			o.ctrlKey || false,
			o.altKey || false,
			o.shiftKey || false,
			o.metaKey || false,
			o.code || null, // keyCode >> no option for "key"
			o.code || null, // charCode
			o.button || 0,
			o.relatedTarget || null
		);
		return keyboardEvent;
	}
	MouseEventPolyfill.prototype = Event.prototype;
	window.KeyboardEvent = KeyboardEventPolyfill;
})(window);
(function (window) {
	if (typeof MouseEvent == "function") { return; } // no need
    // Polyfills DOM4 MouseEvent
	var MouseEventPolyfill = function (type, o) {
		o = o || { bubbles: true, cancelable: true };
		var mouseEvent = document.createEvent('MouseEvent');
		mouseEvent.initMouseEvent(type, 
			o.bubbles,
			o.cancelable,
			window,
			0,
			o.screenX || 0,
			o.screenY || 0,
			o.clientX || 0,
			o.clientY || 0,
			o.ctrlKey || false,
			o.altKey || false,
			o.shiftKey || false,
			o.metaKey || false,
			o.button || 0,
			o.relatedTarget || null
		);
		return mouseEvent;
	}
	MouseEventPolyfill.prototype = Event.prototype;
	window.MouseEvent = MouseEventPolyfill;
})(window);

// initializes gesture and click puppet
!function() {
	DocReady(function() { // set all that up asynchronously not to block page loading
		let _rect = GetElementRect("body");
		document.width = _rect.width;
		document.height = _rect.height;
		document.top = _rect.top;
		document.bottom = _rect.bottom;
		window.width = Polyfill.windowWidth();
		window.height = Polyfill.windowHeight();
		// device detection: first try WURFL as always updated, then RegEx
		cdnSafeLoad("//wurfl.io/wurfl.js", function(){
			let mobile_wurfl = false;
			if (typeof WURFL !== 'undefined') {
				if (isSet(WURFL.is_mobile)) { mobile_wurfl = WURFL.is_mobile; }
				if (isSet(WURFL.form_factor)) { Device.type = WURFL.form_factor; }
				if (isSet(WURFL.complete_device_name)) { Device.name = WURFL.complete_device_name; }
			}
			let mobile_test = (/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|ipad|iris|kindle|Android|Silk|lge |maemo|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i.test(navigator.userAgent) 
				|| /1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(navigator.userAgent.substr(0,4)));
			if (mobile_wurfl || mobile_test) {
				Device.isMobile = true;
				Device.type = "Mobile";
			} else {
				Device.isDesktop = true;
				Device.type = "Desktop";
			}
			Device.recalculate = function() {
				let w = Polyfill.windowWidth();
				let h = Polyfill.windowHeight();
				window.width = Polyfill.windowWidth();
				window.height = Polyfill.windowHeight();
				if (isNumber(h) && h > 0) {
					Device.ratio = (isNumber(h) && h > 0) ? w/h : null;
					if (Device.ratio > 1) {
						Device.horizontal = true;
						Device.vertical = false;
					} else {
						Device.horizontal = false;
						Device.vertical = true;
					}
				}
				if (typeof DeviceRecalculateHook !== "undefined"
					&& isFunction(DeviceRecalculateHook)) {
					DeviceRecalculateHook();
				}			
			}
			Device.recalculate();
			window.on("resize", Device.recalculate, true);
		});
		Scroll.intoViewOptionSupport = function() {
			return isSmoothScrollSupported = 'scrollBehavior' in document.documentElement.style;
		}
		Scroll.intoView = function(el) {
			if (Scroll.intoViewOptionSupport() == true) { // could use a try / catch as well
				element.scrollIntoView({behavior: "smooth", block: "end", inline: "nearest"});		
			} else {
				element.scrollIntoView(false);
			}
		}
		Scroll.preventDefault = function(e) {
			e = e || window.event;
			if (e.preventDefault)
				e.preventDefault();
			e.returnValue = false;	
		}
		Scroll.keydown = function(e) {
			for (var i = Scroll.keys.length; i > 0; i--) {
				if (e.keyCode === Scroll.keys[i]) {
						Scroll.preventDefault(e);
						return;
				}
			}
		}
		Scroll.wheel = function(e) {
			Scroll.preventDefault(e);
		}
		Scroll.disable = function() {
			// TODO: replace with onscroll?
			window.on("wheel", Scroll.wheel);
			document.on("wheel", Scroll.wheel);
			document.on("keydown", Scroll.keydown);
			Scroll.disableMobile();
		}
		Scroll.enable = function() {
			window.removeEventListener('wheel', Scroll.wheel, false);
			document.removeEventListener('wheel', Scroll.wheel, false);
			document.removeEventListener('keydown', Scroll.wheel, false);
			Scroll.enableMobile();
		}
		Scroll.disableMobile = function() {
			document.on("touchmove", Scroll.preventDefault);
		}
		Scroll.enableMobile = function() {
			document.removeEventListener('touchmove', Scroll.preventDefault);
		}
		Scroll.left = function(px = null) {
			if (typeof px === "number") { // setter
				Scroll.to(px);
			} else { // getter
				return Polyfill.scrollLeft();
			}
		}
		Scroll.top = function(px = null) { // Scroll.top(0) to go top
			if (typeof px === "number") { // setter
				Scroll.to(0, px);
			} else { // getter
				return Polyfill.scrollTop();
			}
		}
		Scroll.to = function(x = null, y = null, cb = null, duration = 500, step = 25) {
			if (typeof x !== "number" && typeof y !== "number") { return; } // no valid coordinates passed
			x = typeof x !== "number" ? Scroll.left() : x;
			y = typeof y !== "number" ? Scroll.top() : y;
			let start = { x: Scroll.left(), y: Scroll.top() };
			let change = { x: (x - start.x), y: (y - start.y) };
			// console.log("start:", start.x, start.y);
			let time_now = 0;
			var animateScroll = function() {
				time_now += step;
				// move the document.body
				let shift = {
					x: Math.easeInOutQuad(time_now, start.x, change.x, duration),
					y: Math.easeInOutQuad(time_now, start.y, change.y, duration)
				}
				// polyfill setters
				Polyfill.scrollTop(shift.y);
				Polyfill.scrollLeft(shift.x);
				// do / recurse the animation unless its over
				if (time_now < duration) {
					requestAnimFrame(animateScroll);
				} else {
					if (cb && typeof(cb) === 'function') { cb(); }
				}
			};
			animateScroll();
		}
		Scroll.toElement = function(el = null , cb = null, duration = 750, step = 25) {
			el = GetElement(el);
			// console.log("element:", el);
			if (el == null) { return; }
			let offset = {
				x: GetElementRect(el).left - GetElementRect("body").left,
				y: GetElementRect(el).top - GetElementRect("body").top
			}
			if (!isSet(Device) || Device.horizontal) {
				let header_rect = GetElementRect("._navbar"); // the navbar is the only component inside header
				if (header_rect.height) { offset.y -= header_rect.height; }
			}
			// console.log("scrolling to:", offset.x, offset.y);
			Scroll.to(offset.x, offset.y, cb, duration, step);
		}
		Gesture.Listen = function(el_id) {
			Gesture.el = document.querySelector(el_id);
			Gesture.el.ontouchstart(function(e) {
					Gesture.start.x = e.changedTouches[0].screenX;
					Gesture.start.y = e.changedTouches[0].screenY;
			});
			Gesture.el.ontouchend(function(e) {
					Gesture.end_x = e.changedTouches[0].screenX;
					Gesture.end_y = e.changedTouches[0].screenY;
					Gesture.Handle(Gesture.start.x, Gesture.start.y, Gesture.end_x, Gesture.end_y);
			});
		}
		Gesture.Handle = async function(_x1, _y1, _x2, _y2) {
			let is_callback = (typeof Gesture.Swipe === 'function'); // cb call is conditional
			let x_ratio = ((_x2 - _x1) / Gesture.el.offsetWidth);
			let y_ratio = -((_y2 - _y1) / Gesture.el.offsetHeight);
			// console.log(Gesture.w, Gesture.h, "xr:",x_ratio,"yr",y_ratio);
			if (Math.abs(x_ratio) > Math.abs(y_ratio) && x_ratio > 0.05) {// Gesture.Rec = "swipe-right";
					if (is_callback) { Gesture.Swipe("right"); }
			}
			if (Math.abs(x_ratio) < Math.abs(y_ratio) && y_ratio > 0.05) {// Gesture.Rec = "swipe-up";
					if (is_callback) { Gesture.Swipe("up"); }
			}
			if (Math.abs(x_ratio) > Math.abs(y_ratio) && x_ratio < -0.05) {// Gesture.Rec = "swipe-left";
					if (is_callback) { Gesture.Swipe("left"); }
			}
			if (Math.abs(x_ratio) < Math.abs(y_ratio) && y_ratio < -0.05) {// Gesture.Rec = "swipe-down";
					if (is_callback) { Gesture.Swipe("down"); }
			}
		}
		Underlay.el = document.querySelector(".underlay");
		Overlay.el = document.querySelector(".overlay");
		if (Overlay.el) {
			Overlay.On = function(style = "block") { Overlay.el.style.display = style; } // style can be flex..;
			Overlay.Off = function() { Overlay.el.style.display = "none"; } // need to be safer
			Overlay.Toggle = function() {
				if (Overlay.el.style.display() == "block") { Overlay.Off() } else { Overlay.On(); }
			}
			Gesture.Listen("body");
			// let _dim = Gesture.el.getBoundingClientRect();
			// console.log("dim:",_dim);
		}
	});
}();

/*********************************************
 * NUMERIC TRANSFORM: DATES & MONETARY UNITS *
 *********************************************/

// format number (digits/decimals)
function FormatNumber(n, i_digits, f_digits) {
	if (isNaN(n)) { return; }
	return n.toLocaleString(undefined, {
		minimumIntegerDigits: i_digits,
		minimumFractionDigits: f_digits
	});
}
// get ordinal number as string
function ToOrdinal(i) {
		var j = i % 10,
				k = i % 100;
		if (j == 1 && k != 11) {
				return i + "st";
		}
		if (j == 2 && k != 12) {
				return i + "nd";
		}
		if (j == 3 && k != 13) {
				return i + "rd";
		}
		return i + "th";
}

// returns proper local date from epoch (timestamp)
function epochToLocaleDate(epoch, language = "en-US", opt = null) {
	// opt = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' };
	let dt = new Date(epoch);
	return dt.toLocaleDateString(language, opt);
}

// returns proper string formatted date from epoch
// TODO: implement other languages as full text and improve
function epochToDate(epoch, readable = false, jump = [], language = "en-US") {
	let dt = new Date(epoch);
	function pad(n) { return n < 10 ? "0" + n : n; }
	if (readable) {
		let _m = [
			"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
		];
		let _d = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
		let date_full =
			(jump.includes("day") ? "" : (_d[dt.getDay()] + " "))
			+ (jump.includes("ordinal") ? "" : (ToOrdinal(dt.getDate()) + " "))
			+ (jump.includes("month") ? "" : (_m[dt.getMonth()]) + " ")
			+ (jump.includes("year") ? "" : dt.getFullYear());
		return date_full;
	}
	switch (language) {
		case "en-US":
			return [pad(dt.getMonth()+1), pad(dt.getDate()), dt.getFullYear()].join('/');
		break;
		case "en-UK":
		case "de-DE":
		case "fr-FR":
			return [pad(dt.getDate()), pad(dt.getMonth()+1), dt.getFullYear()].join('/');
		break;
	}
}

/**************
 * USER AGENT *
 **************/

function isLocalhost($whitelist = ["127.0.0.1", "::1", "localhost"]) {
    return ($whitelist.indexOf(window.location.hostname) > -1);
}

// TODO: wrap in a common namespace to prevent overrides
// get href and location related information >> domain / protocol etc
function _location(uri = null) { return new URL(uri ? uri : window.location); }
function _url(uri = null) { return _location(uri).href; }
function _url_no_query(uri = null) { return _location(uri).href.split('?')[0]; }
// alterative: var url = [location.protocol, '//', location.host, location.pathname].join('');
function Protocol(uri = null) { return _location(uri).protocol; } // = Method = Scheme
function DomainName(uri = null) { return _location(uri).hostname; } // = HostName
function Port(uri = null) { return _location(uri).port; }
function Path(uri = null) { return _location(uri).pathname; }
function Domain(uri = null) {
	let url = _location(uri);
	return url.protocol + '//' + url.hostname + (url.port ? ':' + url.port: '');
}

// get query parameter, looks into the document url by default
function QueryParam(key, uri = null) {
	let url = _location(uri);
	if (isSet(uri) && uri.length) {
		uri = uri.replace(/['"]+/g, '');
	}
	// console.log("uri:", uri);
	// QueryParams = new URL(decodeURIComponent(url)).searchParams;
	// return QueryParams.get(key);
	let params = new URLSearchParams(url.search);
	return (params.get(key));
}

// change visible uri
function UpdateURL(to, replace = false, new_stateObj = { id: "" }, new_title = "") {
	replace
	? history.replaceState(new_stateObj, new_title, to)
	: history.pushState(new_stateObj, new_title, to);
}

// add a listener to the URL change event
function OnURLUpdate(f) { // f(event) >> can play with history.state...
	window.onpopstate(f);
}
// redirect user
function Redirect(to, replace = false) {
	to = encodeURI(decodeURI(to)); // sanitizes the URI
	if (typeof RedirectHook !== "undefined" && isFunction(RedirectHook)) { RedirectHook(to, replace); }
	to = to.replace(/['"#]+/g, '').replace("\\", "/").trim();
	if (to.indexOf("/") == -1 && to[0] != "#" && to[0] != ".") {
		to = "#" + to; // defaults to anchor redirect
	}
	let internal = to[0] == '/' || to[0] == "#" || to[0] == "." //relative redirection
	|| (to.indexOf(Domain()) > -1); // absolute redirection to our domain
	// console.log("redirecting to:", to, "internal:", internal);
	if (to[0] == "#" || to[0] == ".") {
	// redirect to element on the same page >> scroll to anchor
		// console.log("scrolling to anchor:", to);
		Scroll.toElement(to);
	} else {
		let pathname = Path();
		// easy anchor same page redirect
		let anchor_start = to.indexOf(pathname);
		let n = to.length;
		if (anchor_start > -1 && internal) {
			let anchor =
			anchor_start < n ? to.substring(anchor_start, to.length())
			: null;
			anchor ? Scroll.toElement(anchor) : Scroll.top(0); // i.e. home from home
		} else {
			// more complex redirect anchor >> query string and / or any
			let domain = isLocalhost()
			? (Domain() + "/" + pathname.split('/')[1]) : Domain();
			let destination = internal ? domain + to : to;
			let current = _url_no_query();
			if (current[current.length - 1] == '/') {
				current = current.slice(0, current.length - 1);
			}
			// redirect on same page
			if (_url_no_query(destination) == current) {
				let anchor = QueryParam("redirect", destination);
				if (!anchor) { // redirect to this page without anchor >> top of body
					Scroll.top(0);
				} else {
					if (anchor[0] != "." && anchor[0] != "#") { anchor = "#" + anchor; }
					Redirect(anchor);
				}
				return;
			}
			// redirect to other page / website
			// loc.replace: similar behavior as an HTTP redirect
			// loc.assign: similar to click >> href redirect (usual dom redirect)
			// >> identical to window.location.href = destination
			replace
			? window.location.replace(destination)
			: window.location.assign(destination);
		}
	}
}

// get raw encoded cookies as string
function RawCookies() {
	let cookies_list = document.cookie.split(';');
	let concat = '';
	for (let i = 1; i <= cookies_list.length; i++) {
		concat += i + ' ' + cookies_list[i-1] + "\n";
	}
	return concat;
}

// browser detection (consistent / non usual)
function BrowserName() {
	if (!!window.chrome && !!window.chrome.webstore) {
		return "chrome";
	} // Chrome 1+
	if (/constructor/i.test(window.HTMLElement) || (function (p) { return p.toString() === "[object SafariRemoteNotification]"; })(!window['safari'] || (typeof safari !== 'undefined' && safari.pushNotification))) {
		return "safari";
	} // Safari 3.0+ "[object HTMLElementConstructor]"
	if (typeof InstallTrigger !== 'undefined') {
		return "firefox";
	} // Firefox 1.0+ 
	if ((!!window.opr && !!opr.addons) || !!window.opera || navigator.userAgent.indexOf(' OPR/') >= 0) {
		return "opera";
	} // Opera 8.0+
	if (!isIE && !!window.StyleMedia) {
		return "edge";
	} // Edge 20+
	if (/*@cc_on!@*/false || !!document.documentMode) {
		return "ie";
	} // Internet Explorer 6-11
	return undefined;
	///if ((isChrome || isOpera) && !!window.CSS; // Blink engine detection
}

/********
 * FORM *
 ********/

function FormReset(form_id) {
	document.getElementById(form_id).reset();
}

function FormParams(form_id) {
	return Array.from(new FormData(document.getElementById(form_id)),
		e => e.map(encodeURIComponent).join('=')).join('&')
}

function FormSubmit(form_id, method, action, destination, content_type, cb) {
	// SubmitForm("POST", "/post_log_in", "application/x-www-form-urlencoded");
	let xhr = new XMLHttpRequest();
	xhr.open(method, destination, true);
	xhr.setRequestHeader("Content-type", content_type);
	xhr.send(/*"action=" + action + "&" + */FormParams(form_id));
	xhr.onload = cb;
}

/***********
 * GENERAL *
 ***********/

// threading mutex
class LockBuffer {
	constructor(size, cb) {
	if (isNaN(size) || size < 1) { console.log("error: LockBuffer size is not a valid number"); return; }
	if (typeof cb !== "function") { console.log("error: LockBuffer callback function missing"); return; }
	this.cb = cb;
	this.locks = [];
	this.locks.length = size; // give array dimension
	this.locks.fill(0); // initialize array of n zeros
	}
	set(i = null) {
	if (i == null || isNaN(i)) {
		i = 0;
		while ((i+1) <= this.locks.length && this.locks[i+1] == 0) { i++ };
	}
	this.locks[i] = 1;
	// if all instances are set, callback
	if (!this.locks.includes(0)) { this.cb(); }
	}
}

// dynamically load libraries from cdn (use if local not available)
async function cdnSafeLoad(url, cb) {
	let script = document.createElement("script");
	script.type = "text/javascript";
	script.src = url;
	script.async = true; // load asynchronously
	script.onload = cb;
	document.body.appendChild(script);
}

// asynchronous call to sleep makes setTimeout more imperative style
function sleep(ms) {
		return new Promise(resolve => setTimeout(resolve, ms));
}

function json2query(p) {
	let p_str = null;
	if (!isEmpty(p)) {
		p_str = "?";
		let size = Object.keys(p).length;
		let i = 0;
		for (let key in p) {
			i++;
			if (p.hasOwnProperty(key)) {
				let val = p[key];
				p_str += key + "=" + val;
				if (i != size) { p_str += "&"; }
			}
		}
	}
	return encodeURI(p_str);
}

function query2json(uri = null) {
	let url = _location(uri);
	let pairs = url.search.slice(1).split('&');
	let result = {};
	pairs.forEach(function(pair) {
		pair = pair.split('=');
		result[pair[0]] = decodeURIComponent(pair[1] || '');
	});
	return JSON.parse(JSON.stringify(result));
}

// custom ajax function, similar to jquery
function _ajax(opt) { // async ?
	if (!isSet(opt) || isObjectEmpty(opt)) { return null; }
	SetDefaults(opt, {
		method: "GET", // GET/POST/PUT/PATCH/DELETE
		url: null,
		headers: {}, // object of key / values to be set as headers
		queryString: "", // querystring already put together
		queryParameters: {}, // query parameters as keys & values
		async: true,
		onprogress: null, // function with parameter xhr.status
		onload: null, // function with 2 arguments: xhr.status and xhr.response
		onredirect: null,
		oninformation: null,
		onsuccess: function(status, res) { console.log("default ajax success", status); },
		onerror: function(status, res) { console.log("default ajax error", status); }
	});
	// console.log("request options:", opt);
	// TODO: implements safety checks (valid url / headers...)
	let xhr = new XMLHttpRequest(); // type ProgressEvent
	// set request headers
	if (!isObjectEmpty(opt.headers)) {
		for (let header in opt.headers) {
			if (opt.headers.hasOwnProperty(header)) {
				let value = opt.headers[header];
				xhr.setRequestHeader(header, value);
			}
		}
	}
	if (!isObjectEmpty(opt.queryParameters)) {
		let p_str = json2query(opt.queryParameters);
		// add the newly created queryString to the existing or replace it
		opt.queryString = isSet(opt.queryString) ? (opt.queryString + "&" + p_str)
		: p_str;
	}
	// console.log("url:", opt.url + opt.queryString);
	xhr.open(opt.method, opt.url + opt.queryString, opt.async);
	if (opt.onprogress) {
		xhr.onprogress = function() { opt.onprogress(xhr.status); }
	}
	if (opt.onerror) {
		xhr.onerror = function() { opt.onerror(xhr.status); }
	}
	// onload is being called for successful AND failed requests
	xhr.onload = function() {
		if (opt.onload) {
			opt.onload(xhr.status, xhr.response);
		}
		switch(Math.floor(xhr.status / 100)) {
			// Informational response, codes 1XX
			case 1:
				if (opt.oninformation) {
					opt.oninformation(xhr.status);
				}
				break;
			// Success, codes 2XX
			case 2:
				if (opt.onsuccess) {
					opt.onsuccess(xhr.status, xhr.response);
				}
				break;
			// Redirection, codes 3XX
			case 3:
				if (opt.onredirect) {
					opt.onredirect(xhr.status, xhr.response);
				}
				break;
			// Client Error, codes 4XX
			case 4:
				if (opt.onerror) {
					opt.onerror(xhr.status);
				}
				break;
			// Server Error, codes 5XX
			case 5:
				if (opt.onerror) {
					opt.onerror(xhr.status);
				}
				break;
		}
	}
	// xhr.onerror = onerror("xhr request failed");
	// xhr.onerror = onerror("xhr request failed");
	xhr.send();
}

// custom ajax / socket requester
async function _API(channel, method, path, onload, onerror/*, onabort*/) {
	//channel: websocket/xhr/fetch
	//method: GET/POST/PUT/PATCH/DELETE
	//path: /api/v1/...
	//onload: function(response){ console.log(response);... }
	if (channel.toLowerCase() == "ajax" || channel.toLowerCase() == "xhr") {
		let xhr = new XMLHttpRequest();
		xhr.open(method, path, true);
		xhr.onload = function() { onload(xhr.response); }
		// xhr.onerror = onerror("xhr request failed");
		// xhr.onerror = onerror("xhr request failed");
		xhr.send();
	} else if (channel.toLowerCase() == "websocket" || channel.toLowerCase() == "ws") {
		let wsw = new WebSocketWrapper(VisitorID, "api");
		wsw.DefaultCallbacks();
		wsw.OnMessage(onload(e.data/*responseText*/));
		wsw.OnError(onerror("websocket request failed"));
		wsw.Send("{ method: " + method + ", path: " + path + " }"); // send JSON formatted WebSocket request to server 
	} else if (channel.toLowerCase() == "fetch") {
		// fetch method to implement (similar to xhr with promise)
	}
}

// checks if var is defined and different than null
function isSet(v) {	return (typeof v !== "undefined" && v != null && v != ""); }
function isEmpty(o) { return (isArrayEmpty(o) && isObjectEmpty(o)); }// could use bitwise &
function isObjectEmpty(o) {
	if (!isSet(o) || !isObject(o)) { return true; }
	for(let key in o) {
		if (o.hasOwnProperty(key)) {
			return false;
		}
	}
	return true;
}
function isArrayEmpty(a) {
	if (!isSet(a) || !isArray(a) || a.length == 0) { return true; }
	return false;
}
// type checks
function isNumber(n) { return !isNaN(n); }
function isInt(n) { return (typeof n === "number" && n.constructor == Number && n % 1 === 0); }
function isBool(n) { return (typeof n === "boolean" && n.constructor == Boolean); }
function isFloat(n) { return (typeof n === "number" && n.constructor == Number && n % 1 !== 0); }
function isString(s) { return (typeof s === "string" && !!s && s.constructor === String); }
function isObject(o) { return (typeof o === "object" && !!o && o.constructor === Object); }
function isFunction(f) { return (typeof f === "function" && !!f && f.constructor === Function); }
function isMouseEvent(e) { return (isObject(e) && e.constructor === MouseEvent); }
function isKeyboardEvent(e) { return (isObject(e) && e.constructor === KeyboardEvent); }
function isTouchEvent(e) { return (isObject(e) && e.constructor === TouchEvent); }
function isArray(o) { return (typeof o === "object" && !!o && o.constructor === Array); }
function isHTMLCollection(o) { return HTMLCollection.prototype.isPrototypeOf(o); }
function isNodeList(o) { return NodeList.prototype.isPrototypeOf(o); }
function isNode(o){
	return (
		typeof Node === "object" ? o instanceof Node : 
		o && typeof o === "object" && typeof o.nodeType === "number" && typeof o.nodeName==="string"
	);
}
function isElement(o){
	return (
		typeof HTMLElement === "object" ? o instanceof HTMLElement : //DOM2
		o && typeof o === "object" && o !== null && o.nodeType === 1 && typeof o.nodeName==="string"
	);
}
// checks if a value from the array matches with in str
function arrayMatch(str, array) {
	for (let i in array) {
		if (str.indexOf(array[i]) > -1) { return array[i]; }
	}
	return "";
}
// checks if a value from the object matches with str
function objectValueMatch(str, obj, key = null) {
	for (let i in obj) {
		if (isArray(obj[i])/* && !key &&*/) {
			let tmp = arrayMatch(str, obj[i]);
			if (tmp) { return tmp }
		} else { // TODO: implement generic (without key)
			if (key in obj[i] && str.indexOf(obj[i][key]) > -1) {
				return obj[i];
			}
		}
	}
	return "";
}

function objToArray(obj, key = null, bkey = null, bval = null) { // blacklists keys / values should be obj / array
	let tmp = [];
	for (let i in obj) {
		if (key in obj[i] && !(bkey && (bkey in obj[i]) && obj[i][bkey] == bval)) { tmp.push(obj[i][key]); }
	}
	return tmp.length > 0 ? tmp : null;
	// if (dim == 1) { return Object.keys(obj); } // lvl 1 keys
	// if (dim == 2) {
	// }
	// TODO: go deeper recursively 
}
// returns boolean depending on whether the string is part of one of the lists
function listCompliant(str, blacklist, whitelist) {
	return ((!blacklist || (isArray(blacklist) && blacklist.indexOf(str) == -1)) // str not on blacklist
		&& (!whitelist || (isArray(whitelist) && whitelist.indexOf(str) > -1))); // str on whitelist
}

// imitates PHP's implode(), works for both objects and arrays (jumps prototypes)
function implode(arr, blacklist = null, whitelist = null) {
	// console.log(arr);
	let cat = "";
	if (typeof arr !== "undefined"/* && typeof arr.length !=="undefined" && arr.length > 0*/) {
			// console.log(arr.name, arr.sport, arr.views);
		for(let key in arr) {
			if(typeof key !== 'undefined' && arr.hasOwnProperty(key) /* && typeof arr[key] !== 'undefined' && arr[key] && arr[key] != ""*/) {
				if (typeof arr[key] === 'object') {
					if (listCompliant(key, blacklist, whitelist)) {
						/*if (isArray(arr[key]) && arr[key].length > 0) {
							console.log("isArray:yes for", key);
							cat += arr[key].join(","); // TODO: explode inside of array >> achieved by treating array as object
						} else */
						if (key != "__proto__" && key != "<prototype>") { // exclude prototypes >> infinite loop
							cat += implode(arr[key], blacklist, whitelist);
						}
					}
				} else if (listCompliant(key, blacklist, whitelist)) {
					cat += " " + arr[key].toString().trim();
					// console.log(arr[key].toString());
				}
			}
		}
	}
	// console.log(cat);
	return cat.trim();
}

// easing functions http://goo.gl/5HLl8
Math.easeInOutQuad = function(t, b, c, d) {
	t /= d / 2;
	if (t < 1) {
		return c / 2 * t * t + b
	}
	t--;
	return -c / 2 * (t * (t - 2) - 1) + b;
};

Math.easeInCubic = function(t, b, c, d) {
	var tc = (t /= d) * t * t;
	return b + c * (tc);
};

Math.inOutQuintic = function(t, b, c, d) {
	var ts = (t /= d) * t,
		tc = ts * t;
	return b + c * (6 * tc * ts + -15 * ts * ts + 10 * tc);
};

// Check functions
function isEmail(e) { // same but without errors
	var regex = new RegExp(/^([a-zA-Z0-9_.+-])+\@(([a-zA-Z0-9-])+\.)+([a-zA-Z0-9]{2,4})+$/);
	return regex.test(e);
}

// strip phone number
function CleanPhone(s) {
	const allowed_reg = new RegExp(/-|_|\s|\.|\/|\(|\)|~/g); // \s = whitespace chars // leave out the +
	s = s.toString().replace(allowed_reg,'');
	if (isNaN(Number(s))) { return null; }
	return s;
}

// strip everything but characters / numbers and spaces:
function CleanName(o) {
	if (!(isObject(o) && isSet(o.name))) { return; }
	SetDefaults(o, { name: "", keep_special: false, latinize: true });
	const allowed_reg = (isSet(o.keep_special) && o.keep_special)
	? new RegExp(/[^\w\d\S ]/g) : new RegExp(/[^\w\d\s ]/g); // \s = whitespace chars \S = all latin
	let s = o.name;
	if (!isSet(s)) { return null; }
	if (o.latinize) { s = Latinize(s); }
	s = s.replace(allowed_reg,'');
	if (!isSet(s)) { return null; }
	return s;
}

// input checks
// phone number compliance checks
// TODO: add country code verification
function CheckPhone(o = { number: "" }) {
	if (!(isObject(o) && isSet(o.number))) { return; }
	let s = o.number;
	s = CleanPhone(s);
	if (!s) { return [false, "Invalid characters in the phone number"]; }
	let n = s.length;
	if (n > 15) { // max number length following E.164
		return [false, "Invalid phone number"];		
	} else if (n < 7) {
		return [false, "Invalid phone number"];
	}
	return [true, null];
}

// email compliance checks
function CheckEmail(o = {s: "" }) { // [bool, error]
	let err = "Email Address";
	if (!(isObject(o) && isString(o.s))) { return [false, "Invalid " + err]; }
	let e = o.s;
	let i = 0;
	let n = e.length;
	// length check, should be locked on front-end
	if (n > 128) { return [false, "Invalid " + err]; }
	// format check >> needs an @, TODO: stronger checks with domain black/whitelisting
	for (i = i; i < n; i++) {
		if (e[i] == '@') {
			if (i > 0) { i++; break; } else {
				return [false, "Email Address invalid"];
			}
		}
		if (e[i] < '!' || e[i] > '~') {
			return [false, "Invalid " + err];
		}	// non printable >> invalid
	}
	if (i == n) { return [false, err + " Missing Domain"]; }
	// host alpha check
	for (i = i; i < n; i++) {
		if (e[i] == '.') { i++; break; }
		if (!((e[i] >= 'a' && e[i] <= 'z') || (e[i] >= 'A' && e[i] <= 'Z')
			|| e[i] == '-' || e[i] == '_')) {
			return [false, "Invalid " + err + " Hostname"];
		}
	}
	if (i == n) { return [false, err + " Missing ccTLD"]; }
	// domain alpha check
	for (i = i; i < n; i++) {
		if (!((e[i] >= 'a' && e[i] <= 'z') || e[i] == ".")) {
			return [false, err + " ccTLD"];
		}
	}
	if (i < 5) { return [false, "Invalid " + err]; }
	return [true, null];
}

function CheckMixed(o = { s: "" }) { // [valid, type]
	if (!(isObject(o) && isString(o.s))) { return [false, null]; }
	let s = o.s;
	if (isEmail(s)) { return ([true, "Email"]); }
	if (CheckPhone({ number: s })[0]) { return ([true, "Phone Number"]); }
	return [false, null];
}

function CheckName(o = { s: "" }) {
	if (!(isObject(o) && isString(o.s))) { return [false, "Invalid Name"];; }
	let s = o.s;
	let clean = CleanName({ name: s}); // get rid of special chars but latinize first
	if (!clean || clean.length < 2) {
		return [false, "Invalid Name"];
	}
	// if (clean.length / s.length < 0.5) {
	if (clean.length != s.length) {
		return [false, "Special characters forbidden"];	
	}
	return [true, null];
}

// password compliance checks
function CheckPassword(o) { // [bool, error]
	if (!(isObject(o) && isString(o.s))) { return [false, "Invalid Password"]; }
	SetDefaults(o, { lower_check: true, upper_check: true, digit_check: false });
	let s = o.s;
	let n = s.length;
	let e = "Password musts ";
	if (n < 8) 	{ return [false, e + "be at least 8 characters"]; }
	if (n > 64) { return [false, e + "be at most 64 characters"]; }
	let upper = false, lower = false, digit = false;
	for (let c of s) {
		if (c >= 'a' && c <= 'z') {
			lower = true // lower case ok
		} else if (c >= 'A' && c <= 'Z') {
			upper = true // upper case ok
		} else if (c >= '0' && c <= '9') {
			digit = true // digit ok
		} //else if (c < '!' || c > '~') { return [false, "Password musts be only printable ascii characters"]; }	// non printable >> invalid
	}
	if (o.lower_check && !lower) { return [false, e + "contain 1 lower case character"]; }
	if (o.upper_check && !upper) { return [false, e + "contain 1 upper case character"]; }
	if (o.digit_check && !digit) { return [false, e + "contain 1 digit"]; }
	return [true, null];
}

function CheckMatch(o) {
	if (!(isObject(o) && isString(o.s))) { return [false, o.ref_type + " do not match"]; }
	SetDefaults(o, { s: "", ref: "", ref_type: "Passwords" });
	if (o.s !== o.ref) {
		return [false, o.ref_type + " do not match"];
	}
	return [true, null];
}

function ASCII_only(s) {	
	if ((s===null) || (s==='')) {
		return false;
	}
	s = s.toString();
	return s.replace(/[^\x20-\x7E]/g, '');
}

// decompose then removes diacritics
function Latinize(s) {
	return s.normalize('NFD').replace(/[\u0300-\u036f]/g, "");
}

//jQuery
function ClosestSibling(el, sib) {

	el = $(el);
	if (!el.length) { return null; }
	let sibling = el.find(sib); 
	let parent = el.parent();
	while (!sibling.length && !parent.is('body')) {
		sibling = parent.find(sib);
		parent = parent.parent();
	}
	if (!sibling.length) { return null; }
	return sibling;
}

// Vue
// Vue.directive('click-outside', {
//   bind: function (el, binding, vnode) {
//     el.clickOutsideEvent = function (event) {
//       // here I check that click was outside the el and his childrens
//       if (!(el == event.target || el.contains(event.target))) {
//         // and if it did, call method provided in attribute value
//         vnode.context[binding.expression](event);
//       }
//     };
//     document.body.addEventListener('click', el.clickOutsideEvent)
//   },
//   unbind: function (el) {
//     document.body.removeEventListener('click', el.clickOutsideEvent)
//   },
// });
