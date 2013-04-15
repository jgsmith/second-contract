class SC.Models.ThingDetail extends Backbone.Model
  defaults: ->
    noun: []
    plural_noun: []
    adjective: []
    plural_adjective: []
    detail: {}

  initialize: ->
    details = []
    for k, d of this.get('detail')
      details.push _.extend({}, d, { id: k })
    @details = new SC.Collections.ThingDetail details
    @details.owner = this

  sync: (method, model, options) ->
    switch method
      when "create", "update"
        data = model.toJSON()
        delete data.id
        @collection?.owner?.get('detail')?[model.id] = data
      when "delete"
        details = @collection?.owner?.get('detail')
        delete details[model.id]
    if options?.success?
      options.success(model, {}, options)

class SC.Collections.ThingDetail extends Backbone.Collection
  model: SC.Models.ThingDetail

class SC.Models.ThingEvent extends Backbone.Model
  defaults: -> { content: "" }

  initialize: ->

  sync: (method, model, options) ->
    switch method
      when "create", "update"
        events = @collection?.owner?.get('events')
        if events
          events[model.id] = model.get('content')
          @collection?.owner?.set(events: events)
      when "delete"
        events = @collection?.owner?.get('events')
        if events
          delete events[model.id]
          @collection?.owner?.set(events: events)
    if options?.success?
      options.success(model, {}, options)

class SC.Collections.ThingEvent extends Backbone.Collection
  model: SC.Models.ThingEvent

class SC.Models.Thing extends Backbone.Model
  defaults: ->
    events: {}
    detail: {
      default: {}
    }

  url: ->
    base = this.collection.url() if this.collection?
    base ?= "/"
    return base + "/" + encodeURIComponent this.id

  initialize: ->
    events = []
    details = []
    for k, e of this.get('events')
      events.push { id: k, content: e }
    @events = new SC.Collections.ThingEvent events
    @events.owner = this
    for k, d of this.get('detail')
      details.push _.extend({}, d, { id: k })
    @details = new SC.Collections.ThingDetail details
    @details.owner = this

  #toJSON: ->
  #  attrs = _.clone(@attributes)
  #  attrs.events = {}
  #  if @events?
  #    @events.each (e) ->
  #      attrs.events[e.id] = e.get('content')
  #  attrs

class SC.Collections.Thing extends Backbone.Collection
  model: SC.Models.Thing
  initialize: (models, options) ->
    @domain = options.domain
    @area = options.area
    @ward = options.ward

  url: ->
    base = "http://localhost:6050/api/worldlib/"
    switch @ward
      when "scene" then   base + "scene/#{@domain}/#{@area}"
      when "path" then    base + "path/#{@domain}/#{@area}"
      when "terrain" then base + "terrain/#{@domain}/#{@area}"
      else base + "thing/#{@domain}/#{@area}/#{@ward}"

