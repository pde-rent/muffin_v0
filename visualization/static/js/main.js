if (typeof LightweightCharts !== "undefined") {
	const chart = LightweightCharts.createChart(document.body, {
		width: Polyfill.windowWidth(),
		height: Polyfill.windowHeight(),
		localization: {
			locale: 'us-US',
			dateFormat: 'yyyy-MM-dd'
		},
		timeScale: {
			// rightOffset: 12,
			// barSpacing: 3,
			// fixLeftEdge: true,
			// lockVisibleTimeRangeOnResize: true,
			// rightBarStaysOnScroll: true,
			// borderVisible: false,
			// borderColor: '#fff000',
			visible: true,
			timeVisible: true,
			secondsVisible: true,
		},
		priceScale: {
			position: 'right',
			mode: 1, // PriceScaleMode. Normal / Logarithmic / Percentage / IndexedTo100
			autoScale: true, // If true, fits ticker.seriesset to the chart size
			// invertScale: false, // If true, a chart series is reflected vertically
			alignLabels: true, // If true, labels with price ticker.series do not overlap
			borderVisible: false, // If true, price scale border is visible
			// borderColor: '#555ffd',
			scaleMargins: {
				top: 0.25,
				bottom: 0.25,
			},
			entireTextOnly: false // If false, top and bottom corner labels are shown even if they are partially not visible
		},
		// handleScroll: {
		// 	mouseWheel: true,
		// 	pressedMouseMove: true,
		// },
		// handleScale: {
		// 	axisPressedMouseMove: true,
		// 	mouseWheel: true,
		// 	pinch: true,
		// },
		crosshair: {
			vertLine: {
				// color: '#6A5ACD', // Crosshair line color
				width: 1.0, // Crosshair line width in pixels
				style: 2, // Solid / Dotted / Dashed / LargeDashed / SparseDotted
				visible: true, // If true, crosshair line is displayed on a chart
				labelVisible: true,
				// labelBackgroundColor: #ffffff,
				// mode: 1 // Magnet / Normal
			},
			horzLine: {
				// color: '#6A5ACD',
				width: 1.0,
				style: 2,
				visible: true,
				labelVisible: true,
				// labelBackgroundColor: #ffffff,
				// mode: 1 // Normal / Magnet
			},
			mode: 2,
		},
		// watermark: {
		// 	color: 'rgba(11, 94, 29, 0.4)',
		// 	visible: true,
		// 	text: 'TradingView Watermark Example',
		// 	fontSize: 24,
		// 	horzAlign: 'left',
		// 	vertAlign: 'bottom',
		// },
		layout: {
			backgroundColor: '#141414',
			textColor: '#f7f7f7',
		},
		grid: {
			vertLines: {
				color: '#756a61',
				style: 1,
				visible: true,
			},
			horzLines: {
				color: '#756a61',
				style: 1,
				visible: true,
			},
		}
	});
	window.on("resize", () => {
		chart.resize(Polyfill.windowHeight(), Polyfill.windowWidth());
	});
	// const lineSeries = chart.addLineSeries();
	// lineSeries.setData([{time:..,value:..}...) i.e.:
	// [
	// 	{ time: '2019-04-11', value: 80.01 },
	// 	{ time: '2019-04-12', value: 96.63 },
	// 	{ time: '2019-04-13', value: 76.64 },
	// ]
	const ohlcSeries = chart.addCandlestickSeries({
		overlay: true,
		title: ticker.name,
		scaleMargins: {
			top: 0.1,
			bottom: 0.1,
		},
		upColor: '#ff8400',
		downColor: '#000',
		borderDownColor: '#ff8400',
		borderUpColor: '#ff8400',
		wickDownColor: '#ff8400',
		wickUpColor: '#ff8400',
	});
	let dt = null;
	for (let i in ticker.series) {
		dt = new Date(parseInt(ticker.series[i].time));
		// ticker.series[i].time = dt.getFullYear() + "-" + dt.getMonth() + "-" + dt.getDay();
		ticker.series[i].time = Math.round(dt.getTime() / 1000);
		ticker.series[i].open = parseFloat(ticker.series[i].open);
		ticker.series[i].high = parseFloat(ticker.series[i].high);
		ticker.series[i].low = parseFloat(ticker.series[i].low);
		ticker.series[i].close = parseFloat(ticker.series[i].close);
		ticker.series[i].volume = parseFloat(ticker.series[i].volume);
	}
	ohlcSeries.setData(ticker.series);
} else {
	console.log("LightweightCharts not loaded");
}
