class SC.Views.DomainList extends Backbone.View
  initialize: ->
    @model.bind 'reset', @render, this
    @model.bind 'add', @appendNewDomain, this
    @template = _.template $('#domain-list-template').html()

  render: ->
    @$el.html @template {}
    @$ul = @$el.find("ul").first()
    for domain in @model.models
      @appendNewDomain domain
    @el

  appendNewDomain: (domain) ->
    @$ul.append(new SC.Views.DomainListItem({model:domain}).render())

$("#new-domain-modal").find(".btn-primary").click (e) ->
  e.preventDefault()
  domain_name = $("#new-domain-modal").find("input[name='name']").val()
  SC.Collections.domains.create
    name: domain_name
  $("#new-domain-modal").modal('hide')
